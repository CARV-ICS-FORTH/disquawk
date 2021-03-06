#!/usr/bin/env ruby

# Check for proper number of command line args.
# expected_args=1
# e_badargs=65

# if ARGV.length < expected_args
#   puts "Usage: #{$0} pipe|<trace_file> [<mapfile> [<mapfile> ...]]"
#   exit e_badargs
# end

# trace=ARGV[0].to_s
romstart=0
alignment=0

# if (trace.equals?("pipe"))
f = open('FormicApp.suite.map')
cstart = f.grep(/canonical start/)[0]
cstart = cstart.split('=')[1].to_i
#puts "\n\nCSTART #{cstart}"
#cstart = 64 - (cstart%64)
f.close

# File.open(trace).each do |line|
ARGF.each do |line|
  if line.start_with?("FormicApp canonical start")
    alignment = line.split('=')[1].to_i
    #puts "\n\nALIGNMENT1 #{alignment}"
    alignment = alignment - cstart
    #puts "\n\nALIGNMENT2 #{alignment}"
    puts line
  # if line.start_with?("*TRACE*:*ROM*:")
  #   romstart = line.split(':')[2].to_i
  #   puts line
  # els
  elsif line.start_with?("*STACKTRACE*:") and not line.include?("\"")
    offset = line.split(':')[1].to_i-romstart
    flag = 0
    begin
      # if offset < 0
      #   puts "rom= " + romstart.to_s
      #   puts "off= " + offset.to_s
      #   puts "par= " + line.split(':')[1]
      #   puts "par= " + line.split(':')[1].to_i(16).to_s
      # end
      cmd = "grep 'METHOD\.#{offset}'"
      if ARGV.length > 1
        i = 1
        begin
          cmd << " "+ARGV[i].to_s
          i += 1
      end until i == ARGV.length
      else
        cmd << " *.map"
      end

      # puts cmd
      l = %x[#{cmd}]
      tokens = l.split(/=/)
      if tokens.length > 1
        puts line.gsub(/\n/, '')+"\t"+tokens[1].split[1]
        flag = 3
      else
        #puts "\n\nOFFSET #{offset}"
        #puts "\n\nALIGNMENT #{alignment}"
        offset -= alignment
        flag += 1
      end
    end until flag > 1
    if flag == 2
      puts line.gsub(/\n/, '')+"\tfailed ("+offset.to_s+")"
    end
  else
    # match ar_backtrace prints "0x01/0 [0] F=    9884 PC=    98F8 fs=48 1:0"
    res = line.match(%r{(?'pre'0x\h{2}/[[:digit:]] \[[[:space:]]*[[:digit:]]+\]) F= {,8}(?'fptr'\h{,8}) PC=(?'pc' {,8}\h{,8}).*})
    if res
      # Find function in dump file (objectdump)
      cmd = "egrep -i '^0*#{res[:fptr]}' ./build/squawk.mb.dump"
      l = %x[#{cmd}]
      begin
        func_name = l.match(%r{\h* <(?'function'\w+)>})[:function]
        puts res[:pre]+" "+res[:pc]+" "+func_name
      rescue
        puts res[:pre]+" "+res[:pc]+" Not found"
      end
    else
      puts line
    end
  end
end
