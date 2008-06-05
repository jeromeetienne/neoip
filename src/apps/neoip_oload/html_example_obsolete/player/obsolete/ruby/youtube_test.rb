#!/usr/bin/ruby
#
# \par Brief Description
# test on youtube login


# the 'require' for the library i use
require 'rubygems'
require 'mechanize'
require 'logger'
require 'pp'

# video_id which require 'confirmation of birth date'
video_id	= "NidUG88wY4o"


################################################################################
#		test age confirmation
# - failed
################################################################################
agent		= WWW::Mechanize.new
agent.user_agent_alias = 'Mac Safari'
html_page	= agent.get("http://youtube.com/watch?v=#{video_id}")
pp html_page.uri.to_s

if( html_page.uri.to_s.match("verify_age") )
	# first login
	login_page	= agent.click html_page.links.with.text("logging in")
	login_form	= login_page.forms.name("loginForm").first
	login_form.username = 'nonymity'
	login_form.password = 'slotapim'
	html_page	= agent.submit(login_form, login_form.buttons.first)
	# now confirm the age
	age_form	= html_page.forms[2];
	html_page	= agent.submit(age_form, age_form.buttons.first);
end

pp html_page.uri.to_s
#pp html_page.body
exit

################################################################################
# sucessfull attempts to get it via login_form
################################################################################

agent		= WWW::Mechanize.new
agent.user_agent_alias = 'Mac Safari'
login_page	= agent.get('http://youtube.com/login')
login_form	= login_page.forms.name("loginForm").first;
login_form.username = 'nonymity';
login_form.password = 'slotapim';
html_page	= agent.submit(login_form, login_form.buttons.first);

pp html_page.uri.to_s
#pp html_page.body

exit

##############################################################################
# example of successfull login
##############################################################################

agent		= WWW::Mechanize.new
agent.user_agent_alias = 'Mac Safari'
login_page	= agent.get('http://youtube.com/login')
# NOTE: IMPORTANT: for unknown reason doing agent.submit fails....
# - seems like a bug in the mechanize... according to wireshark result... unclear tho
html_page	= agent.post("http://youtube.com/login", {
				        'current_form' => 'loginForm',
				        'username'     => 'nonymity',
				        'password'     => 'slotapim',
				        'action_login' => 'Log In'
				    });
pp html_page.uri.to_s
#pp html_page.body
exit












