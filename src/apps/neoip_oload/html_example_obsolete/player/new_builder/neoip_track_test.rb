#!/usr/bin/ruby
#
# \par Brief Description
# to handle a track_pool on blip.tv

# the 'require' for the library i use
require 'rubygems'
require 'json'
require 'pp'

require 'neoip_trackgen_t'
require 'neoip_playlist_updater_t'


trackgen_info_bliptv	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "bliptv",
					"desc"	=> {
						"base_uri" => "http://tedtalks.blip.tv/rss"
						}
				}
		},
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "bliptv",
					"desc"	=> {
						"base_uri" => "http://make.blip.tv/rss"
						}
				}
		},
		{	"cmd"	=> "select_random_contiguous",
			"arg"	=> { "nb_contiguous"	=> 3	}
		}
		];

trackgen_info_youporn	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "youporn",
					"desc"	=> {
						"dummy" => "slota"
						}
				}
		},
		{	"cmd"	=> "select_random_contiguous",
			"arg"	=> { "nb_contiguous"	=> 3	}
		}

		];
trackgen_info_youtube1	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "youtube",
					"desc"	=> {	"cmd"	=> "videos_by_user",
							"arg"	=> {	"user"	=> "seeso"
								}
						}
				}
		},
		{	"cmd"	=> "select_random_contiguous",
			"arg"	=> { "nb_contiguous"	=> 3	}
		}
		];
trackgen_info_youtube2	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "youtube",
					"desc"	=> {	"cmd"	=> "videos_by_tag",
							"arg"	=> {	"tag"	=> "ufo"
								}
						}
				}
		},
		{	"cmd"	=> "select_random_contiguous",
			"arg"	=> { "nb_contiguous"	=> 3	}
		}
		];
trackgen_info_direct	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "direct",
					"desc"	=> [{	"title"	=> "me in front of webcam",
							"uri"	=> "http://jmehost2/~jerome/output.flv"
						},
						{	"title"	=> "Elephant dream",
							"uri"	=> "http://jmehost2/~jerome/flash/elephantsdream_420_v4.flv"
						},
						{	"title"	=> "frank dixon",
							"uri"	=> "http://jmehost2/~jerome/flash/0021_frankdixon.flv"
						},
						{	"title"	=> "3rd planete from the sun 6x01",
							"uri"	=> "http://jmehost2/~jerome/flash/3rd_planete_6x01_40k.flv"
						}
						]
				}
		},
		{	"cmd"	=> "filter_field",
			"arg"	=> { 	"field"		=> "track_title",
					"pattern"	=> "3rd"	}
		}
		];
trackgen_info_direct1	= [
		{	"cmd"	=> "pick_one_randomly",
			"arg"	=> [{	"precedence"	=> 50,
					"trackgen_info"	=> [{	"cmd"	=> "concat",
								"arg"	=> {	"space"	=> "direct",
										"desc"	=> [{	"title"	=> "me in front of webcam",
												"uri"	=> "http://jmehost2/~jerome/output.flv"
											}]
									}
							}]
				},
				{	"precedence"	=> 50,
					"trackgen_info"	=> [{	"cmd"	=> "concat",
								"arg"	=> {	"space"	=> "direct",
										"desc"	=> [{	"title"	=> "Elephant dream",
												"uri"	=> "http://jmehost2/~jerome/flash/elephantsdream_420_v4.flv"
											}
											]
									}
							}]
				}]
		}];
trackgen_info_direct2	= [
		{	"cmd"	=> "concat",
			"arg"	=> {	"space"	=> "direct",
					"desc"	=> [{	"title"	=> "2me in front of webcam",
							"uri"	=> "http://jmehost2/~jerome/output.flv"
						},
						{	"title"	=> "1Elephant dream",
							"uri"	=> "http://jmehost2/~jerome/flash/elephantsdream_420_v4.flv"
						},
						{	"title"	=> "0frank dixon",
							"uri"	=> "http://jmehost2/~jerome/flash/0021_frankdixon.flv"
						},
						{	"title"	=> "3rd planete from the sun 6x01",
							"uri"	=> "http://jmehost2/~jerome/flash/3rd_planete_6x01_40k.flv"
						}
						]
				}
		},
		{	"cmd"	=> "sort_by",
			"arg"	=> { 	"field"	=> "track_title"}
		}
		];


trackgen_info_from_file	= JSON.parse(File.read("plistgen_info/youtube_ufo.plistgen_info"))['trackgen_info']
		
trackgen_info	= trackgen_info_youtube1


track_gen	= Neoip::Trackgen_t.new( trackgen_info )

track_gen.track_list.each { |item| puts "title=#{item.track_title} flv_uri=#{item.flv_uri}" }

jspf_arr	= track_gen.to_jspf_arr()
pp jspf_arr


