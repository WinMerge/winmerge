# MakeExpatEncode.rb
# Created:  2005-10-21, Perry Rapp
# Edited:   2005-10-22, Perry Rapp

	regx = /^0x([0-9a-fA-F]{2})\t0x([0-9a-fA-F]{4})\t#\t/
	Dir['mappings/*/*.TXT'].each do |file_name|
		encoding = file_name[0..-5].gsub(/^.*\//,"")
		encoding.gsub!("-", "_")
		outfilename = "map_data_" + encoding + ".h"
		File.open(file_name, "r") do |aFile|
			File::open( outfilename, 'w') do |f|
				f << "// Created from " + file_name << "\n"
				f << "// ftp://ftp.unicode.org/Public/MAPPINGS/" << "\n"
				f << "// Created " << Date.today.to_s << "\n"
				f << "static int map_data_" + encoding + "[] = {" << "\n"
				aFile.each_line { |line| 
					md = regx.match(line)
					if md
						mapline = "\t0x" + md[1] + ", 0x" + md[2] + ","
						mapline += " // " + md.post_match.chomp
						puts mapline
						f << mapline << "\n"
					end
				}
			f << "\t-1, -1 // mark end of data" << "\n"
			f << "};" << "\n"
			end
		end
	end




