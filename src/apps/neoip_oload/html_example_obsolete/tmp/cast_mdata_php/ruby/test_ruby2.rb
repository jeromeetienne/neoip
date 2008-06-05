#!/usr/bin/env ruby
require 'cgi'

cgi = CGI.new("html4")
params = cgi.params

cgi.out() do
  cgi.html() do
    cgi.head{ cgi.title{"TITLE"} } +
    cgi.body() do
      cgi.pre() do
        CGI::escapeHTML(
          "params: " + cgi.params.inspect + "\n" +
          "cookies: " + cgi.cookies.inspect + "\n" +
          ENV.collect() do |key, value|
            key + " --> " + value + "\n"
          end.join("")
        )
      end
    end
  end
end