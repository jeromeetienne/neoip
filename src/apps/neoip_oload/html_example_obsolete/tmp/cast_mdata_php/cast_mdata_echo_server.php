<?php

// This small php script is a trick to 'echo' the bt_cast_mdata_t on a server
// - this allow to get a simple server running on a inetreach server
//   - neoip-casti may not be inet reach
// - thus neoip-casti can send their bt_cast_mdata_t here and the 
//   neoip-casto can retrieve them here too.
// - WARNING: absolutly no check or security is done
//   - this is prototype only
// - TEST in ruby
//   - require 'xmlrpc/client'
//   - server = XMLRPC::Client.new2("http://localhost/~jerome/oload/cast_mdata_php/cast_mdata_echo_server.php")
//   - server.call("set_cast_mdata", "superstream.flv", { :slota => 'bip', :goup => 'slsdfadfsl' })
//   - server.call("get_cast_mdata", "superstream.flv")
//   - begin; server.call("get_cast_mdata", "superstream.flv"); rescue => e; puts "Received fault code #{e.faultCode} String #{e.faultString}"; end
// - all the data are stored in "/tmp/.neoip_cast_mdata_echo_php." 
//   - this is due to some file permissions stuff
//   - ugly but easy
// -----------------------------
// - how to measure latency
//   - Time.now - Time.at(server.call("get_cast_mdata", "superstream")['casti_date'])
//   - works if done with echo_server and casti clock are insync. like on the same box
//   - usefull to test the casti_date compensation
// -----------------------------
// POSSIBLE IMPROVEMENT:
// - currently neoip-casti has to periodically push the bt_cast_mdata_t into the server
//   at a very high frequency. 
//   - with a maximum of once every 10sec.
//   - and it has to do so even, if nobody read the stream.
//   - this is a very bad stuff
// - an alternative would be to have neoip-casti to have a dhcp kind of protocol
//   with the bt_cast_mdata_t server. and have the server read/cache the bt_cast_mdata_t
//   only when needed.
//   - this would reduce significantly network burden on the server and on neoip-casti
// - this implies to have neoip-casti own bt_cast_mdata_t server to be inetreach
//   - which may be possible or not
//   - if not possible fallback to the push mechanism
// - how to determine when to push and when to register
//   - it needs to be tested by the neoip-casti
//   - what about ?
//     - do registration
//     - read your own bt_cast_mdata_t, if it fails, fallback on push
// - details have to be worked out
// - to get the remote ip address
//   - getenv('REMOTE_ADDR')
//   - usefull to be able to work even if the cast_mdata is on the local network
//   - important for confortable testing


// include the xmlrpc library for php
include('IXR_Library.inc.php');

function do_log($str)
{
	$fp = fopen("/tmp/cast_mdata_echo_server_logs.txt", "a+");
	fputs($fp, $str."\n");
	fclose($fp);
}

function cast_name_to_filepath($cast_name)
{
	return '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
}


function set_cast_mdata_pull($args)
{
	// get parameter from the xmlrpc call
	$cast_name	= $args[0];
	$port_lview	= $args[1];
	$port_pview	= $args[2];
	$uri_pathquery	= $args[3];
	
	// determine the $chosen_port depending on the remote_addr of rfc1918
	// - TODO test the 172.16/12
	$remote_addr	= getenv('REMOTE_ADDR');
	$elems		= explode(".", $remote_addr);
	if( ($elems[0] == "192" && $elems[1] == "168") || $elems[0] == "10"){
		$chosen_port	= $port_lview;
	}else{
		$chosen_port	= $port_pview;
	}
	
	// build casti_srv_uri
	$casti_srv_uri	= "http://".$remote_addr.":".$chosen_port.$uri_pathquery;


	// create the cast_mdata_ctx
	$cast_mdata_ctx	= array(	"name"	=> $cast_name	,
					"type"	=> "pull"	,
					"arg"	=> $casti_srv_uri);

if(1){
	do_log("*********************************");
	do_log("cast_name=".$cast_name);
	do_log("port_lview=".$port_lview);
	do_log("port_pview=".$port_pview);
	do_log("uri_pathquery=".$uri_pathquery);
	do_log("remote_addr=".$remote_addr);
	do_log("chosen_port=".$chosen_port);
	do_log("casti_srv_uri=".$casti_srv_uri);
	do_log("cast_mdata_ctx=".print_r($cast_mdata_ctx, true));
}

	// store the cast_mdata_ctx into the file
	$temp_path	= tempnam("/tmp", "FOO");
	file_put_contents($temp_path, serialize($cast_mdata_ctx));
	
	$file_path	= '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
	rename($temp_path, $file_path);
}

function set_cast_mdata_push($args)
{
	// get parameter from the xmlrpc call
	$cast_name	= $args[0];
	$cast_mdata	= $args[1];

	// create the cast_mdata_ctx
	$cast_mdata_ctx	= array(	"name"	=> $cast_name	,
					"type"	=> "push"	,
					"arg"	=> $cast_mdata);
					
	// store the cast_mdata_ctx into the file
	$file_path	= '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
	file_put_contents($file_path, serialize($cast_mdata_ctx));
}


function get_cast_mdata($args)
{
	// get parameter from the xmlrpc call
	$cast_name	= $args;

	// try to read the cast_mdata_ctx from the file
	$file_path	= cast_name_to_filepath($cast_name);
	// if the file do no exist, return an error
	if( !file_exists($file_path) )	return new IXR_Error(-1, 'stream '.$cast_name.' is unknown (no ctx file '.$file_path.')');
	// read the file content
	$file_data	= file_get_contents($file_path);
	// unseralize the data
	$cast_mdata_ctx	= unserialize($file_data);

	// compute the age of the cast_mdata_ctx in second
	$ctx_age	= time() - filectime($file_path);

	do_log("ctx_age=".$ctx_age);
	do_log("cast_mdata_ctx=".print_r($cast_mdata_ctx, true));

	// forward to the proper handler depending on cast_mdata_ctx['type']
	if( $cast_mdata_ctx['type'] == "pull" ){
		return get_cast_mdata_pull($ctx_age, $cast_mdata_ctx);
	}else{
		return get_cast_mdata_push($ctx_age, $cast_mdata_ctx);
	}
}

function get_cast_mdata_pull($ctx_age, $cast_mdata_ctx)
{
	$cast_name	= $cast_mdata_ctx['name'];
	// sanity check - $cast_mdata_ctx['type'] MUST be "pull"
	assert( $cast_mdata_ctx['type'] == "pull" );
	
	do_log("slota");
	do_log("ctx_age=".$ctx_age);
	do_log("cast_mdata_ctx=".print_r($cast_mdata_ctx, true));
	
	// get the casti_srv_uri directly from $cast_mdata_ctx['arg'];
	$casti_srv_uri	= $cast_mdata_ctx['arg'];
	
	// forward the xmlrpc to casti_srv_uri
	$client		= new IXR_Client($casti_srv_uri);
//	$client->debug	= true;
	if( !$client->query('get_cast_mdata', $cast_name) ){
		// delete the context file if it is now unreachable 
//		unlink(cast_name_to_filepath($cast_name));
		// forward the error to the caller
		return new IXR_Error($client->getErrorCode(), "slotaIXR".$client->getErrorMessage());
	}

	// get the cast_mdata
	$cast_mdata	= $client->getResponse();

	// return the $cast_mdata
	return $cast_mdata;
}

function get_cast_mdata_push($ctx_age, $cast_mdata_ctx)
{
	// sanity check - $cast_mdata_ctx['type'] MUST be "push"
	assert( $cast_mdata_ctx['type'] == "push" );

	// if the ctx_file is older than XXX, delete it and return an error
	if( $ctx_age > 60 ){
		$cast_name	= $cast_mdata_ctx['name'];
		unlink(cast_name_to_filepath($cast_name));
		return new IXR_Error(-1, 'Obsolete stream.');
	}

	// get the cast_mdata directly from $cast_mdata_ctx['arg'];
	$cast_mdata	= $cast_mdata_ctx['arg'];
	
	// attempts to update the casti_date
	// - seems to work ok.. at one-second precision
	// - NOTE: could be improved with more precise timer... likely
	//   - would it be usefull ? 1sec is ok for now
	$cast_mdata['casti_date'] += $ctx_age;
	
	// return the $cast_mdata
	return $cast_mdata;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			delete the state for this cast_name
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function del_cast_mdata($args)
{
	$cast_name	= $args;
	$file_path	= '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
	// if the file do no exist, return an error
	if( !file_exists($file_path) )	return new IXR_Error(-1, 'stream '.$cast_name.' is unknown.');
	// remove this file
	unlink($file_path);
	// return noerror
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		store the content of the cast_mdata in a file
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function set_cast_mdata_old($args)
{
	$cast_name	= $args[0];
	$file_path	= '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
	$file_data	= serialize($args[1]);
	file_put_contents($file_path, $file_data);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		fetch the content of the cast_mdata in a file
// - TODO maybe some stuff to adapt the neoip-casti date
//   - the neoip-casto assume the xmlrpc is passed live and use this to 
//     measure the delta of time between casti and casto
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function get_cast_mdata_old($args)
{
	$cast_name	= $args;
	$file_path	= '/tmp/.neoip_cast_mdata_echo_php.'.$cast_name;
	// if the file do no exist, return an error
	if( !file_exists($file_path) )	return new IXR_Error(-1, 'stream '.$cast_name.' is unknown.');
	// if the file is older than XXX, delete it and return an error
	// - TODO this idle timeout is to be tunned
	if( time() - filectime($file_path) > 60 ){
		unlink($file_path);
		return new IXR_Error(-1, 'Obsolete stream.');
	}
	// read the file content
	$file_data	= file_get_contents($file_path);
	// unseralize the data
	$real_data	= unserialize($file_data);
	
	// attempts to update the casti_date
	// - seems to work ok.. at one-second precision
	// - NOTE: could be improved with more precise timer... likely
	//   - would it be usefull ? 1sec is ok for now
	$real_data['casti_date'] += time() - filectime($file_path);

	// return it as a struct
	return $real_data;
}

/* Create the server and map the XML-RPC method names to the relevant functions */
$server = new IXR_Server(array(
    'set_cast_mdata_old'	=> 'set_cast_mdata_old',
    'get_cast_mdata_old'	=> 'get_cast_mdata_old',

    'del_cast_mdata'		=> 'del_cast_mdata',
    'set_cast_mdata_pull'	=> 'set_cast_mdata_pull',
    'set_cast_mdata_push'	=> 'set_cast_mdata_push',
    'get_cast_mdata'		=> 'get_cast_mdata',
));

?>
