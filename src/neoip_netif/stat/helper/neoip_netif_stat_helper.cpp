/*! \file
    \brief Definition for static helper of the netif_stat_t


*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_netif_stat_helper.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_err.hpp"
#include "neoip_file_dir.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Read the netif_stat_t for a given name
 * 
 * @return the read netif_stat_t or null if an error occured
 */
netif_stat_t	netif_stat_helper_t::one_netif_stat(const std::string &name)	throw()
{
	netif_stat_t	netif_stat;
	datum_t		read_datum;
	file_err_t	file_err;
	file_path_t	filename_prefix	= "/sys/class/net";
	file_path_t	filename_suffix	= "statistics";
	file_path_t	file_path;
	// init the begining of the netif_stat_t
	netif_stat.m_name		= name;
	netif_stat.m_capture_date	= date_t::present();

	// read the rx_byte data
	file_path	= filename_prefix / name / filename_suffix / "rx_bytes";
	file_err	= file_sio_t::readall(file_path, read_datum);
	if( file_err.failed() )	return netif_stat_t();
	netif_stat.m_rx_byte		= atoi(read_datum.to_stdstring().c_str());	

	// read the tx_byte data
	file_path	= filename_prefix / name / filename_suffix / "tx_bytes";
	file_err	= file_sio_t::readall(file_path, read_datum);
	if( file_err.failed() )	return netif_stat_t();
	netif_stat.m_tx_byte		= atoi(read_datum.to_stdstring().c_str());	
	
	// return the resulting netif_stat
	return netif_stat;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a netif_stat_arr_t of all the netif
 */
netif_stat_arr_t	netif_stat_helper_t::all_netif_stat()			throw()
{
	std::vector<std::string>	name_db	= netif_stat_helper_t::all_netif_name();
	netif_stat_arr_t		stat_arr;
	// go thru the all the netif_name of the name_db
	for(size_t i = 0; i < name_db.size(); i++){
		std::string &	netif_name	= name_db[i];
		netif_stat_t	netif_stat;
		// get the netif_stat_t for this netif_name
		netif_stat	= netif_stat_helper_t::one_netif_stat(netif_name);
		// add it to the netif_stat_arr_t
		stat_arr	+= netif_stat;
	}
	// return the just built netif_stat_arr_t;
	return stat_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a std::vector of all the current netif name
 */
std::vector<std::string>	netif_stat_helper_t::all_netif_name()		throw()
{
	std::vector<std::string>	result;
	file_dir_t			file_dir;
	file_err_t			file_err;
	// open the directory
	file_err	= file_dir.open("/sys/class/net");
	if( file_err.failed() )	return std::vector<std::string>();
	// sort the result 
	file_dir.sort();
	// populate the result with the file_dir 
	for( size_t i = 0; i < file_dir.size(); i++ )
		result.push_back( file_dir[i].basename().to_string() );
	// return the just built result
	return result;
}

NEOIP_NAMESPACE_END;






