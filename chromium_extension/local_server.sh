#!/bin/bash
# https://stackoverflow.com/a/24777120
send_message() {
  message="$1"
  # Calculate the byte size of the string.
  # NOTE: This assumes that byte length is identical to the string length!
  # Do not use multibyte (unicode) characters, escape them instead, e.g.
  # message='"Some unicode character:\u1234"'
  messagelen=${#message}
  # Convert to an integer in native byte order.
  # If you see an error message in Chrome's stdout with
  # "Native Messaging host tried sending a message that is ... bytes long.",
  # then just swap the order, i.e. messagelen1 <-> messagelen4 and
  # messagelen2 <-> messagelen3
  messagelen1=$(( ($messagelen      ) & 0xFF ))               
  messagelen2=$(( ($messagelen >>  8) & 0xFF ))               
  messagelen3=$(( ($messagelen >> 16) & 0xFF ))               
  messagelen4=$(( ($messagelen >> 24) & 0xFF ))               
  # Print the message byte length followed by the actual message.
  printf "$(printf '\\x%x\\x%x\\x%x\\x%x' \
        $messagelen1 $messagelpen2 $messagelen3 $messagelen4)%s" "$message"
}
local_server() {
  if pgrep -f 'php -S localhost:8000' > /dev/null; then
    pkill -f 'php -S localhost:8000' & send_message '"Local server off."' 
  else
    php -S localhost:8000 & send_message '"Local server on."'
  fi
}
local_server
