#! /usr/bin/env ruby

open("| dot -Tpng", "w") { |fd|
  fd.puts("graph {")
  ARGF.lines.each { |l|
    n1, n2, g, b, g2, b2 = l.split
    fd.puts("\t#{n1} -- #{n2} [label=\"#{g}/#{b}\"]")
  }
  fd.puts("}")
}

