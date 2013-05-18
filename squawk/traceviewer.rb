#!/usr/bin/env ruby

# Check for proper number of command line args.
expected_args=2
e_badargs=65

if ARGV.length < expected_args
  puts "Usage: #{$0} <trace> <mapfile> [<mapfile> ...]"
  exit e_badargs
end

trace=ARGV[0].to_s

File.open(trace).each do |line|
  if line.start_with?("*STACKTRACE*:")
    offset=line.split(':')[1].to_i-4
    cmd="grep '#{offset} :.*method'"
    i=1
    begin
      cmd << " "+ARGV[i].to_s
      i+=1
    end until i == ARGV.length

    l=%x[#{cmd}]
    tokens=l.split(/.* #{offset}.*method /)
    if tokens.length > 1
      puts line.gsub(/\n/, '')+"\t"+tokens[1].split[1]
    else
      puts line.gsub(/\n/, '')+"\tfailed"
    end
  else
    puts line
  end
end
