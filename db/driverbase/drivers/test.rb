#!/usr/bin/ruby

require 'djonwrapper'

begin
	 con = Djonwrapper::ConnectionManager.getConnection('localhost')
	 con.open();
	 con.insert("rubydb", "ns", "{ name: 'Mary', lastName: 'Johnson'}");

	 v = con.find("rubydb", "ns", "");
	 v.each do |obj|
		 puts obj.toChar()
	 end

	 puts "Testing a different way"

	 b = Djonwrapper::BSONObj.new
	 b.add("name", "John")
	 b.add("lastName", "Smith")
	 b.add("age", 38)

	 con.insert("rubydb", "ns", b)

	 v = con.find("rubydb", "ns", "");
	 v.each do |obj|
		 puts obj.toChar
	 end

	 con.close()

end
