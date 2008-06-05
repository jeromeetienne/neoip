#!/usr/bin/ruby
#
# \par Brief Description
# test on youporn
#
# \par Possible improvement - algo to produce a channel
# 1. get the number of page
# 2. get a random page
# 3. get a random video in this page
# 4. get this video
# 5. get a video related to it
# 5. loop to 4 until the loop is filled


# the 'require' for the library i use
require 'rubygems'
require 'mechanize'
require 'logger'
require 'pp'

agent		= WWW::Mechanize.new
agent.user_agent_alias = 'Mac Safari'
page_ageconfirm	= agent.get('http://youporn.com/')
form= page_ageconfirm.forms[0]
page_init	= agent.submit(form, form.buttons.first)

page_idxmax	= page_init.links.with.href(%r{\?page=}).collect{|link| link.href.split("=")[1]}.max

puts "page_idxmax=#{page_idxmax}"

page_idx	= rand(page_idxmax);
puts "page_idx=#{page_idx}"

page_front	= agent.get("/?page=#{page_idx}")

pp page_front


