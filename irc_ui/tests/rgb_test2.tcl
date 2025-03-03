set txt {}
for {set r 255; set g 0; set b 0} {$g < 256} {incr g} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr g -1
for {} {$r > -1} {incr r -1} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr r
for {} {$b < 256} {incr b} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr b -1
for {} {$g > -1} {incr g -1} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr g
for {} {$r < 256 } {incr r} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr r -1
for {} {$b > -1} {incr b -1} {
  append txt "\006C#[format %02X $r][format %02X $g][format %02X $b]\006#"
}
incr b

IRC::echo $txt
unset txt
