#!/usr/bin/expect -f
# 03
# test checks the reuse of deleted books
set timeout -1
set programName "library"
set filename "test"

# delete all files starting with test
# file delete [glob test*]
spawn rm -f $filename.db $filename.ind $filename.lst

# call program
spawn valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$programName worst_fit test
expect "Type command and argument/s."

# add first book
send  "add 12346|978-2-12345680-3|EL ingenioso hidalgo Quijote de la Mancha|Santillana\r"
expect "Record with BookID=12346 has been added to the database"

# add second book
send  "add 12345|978-2-12345086-3|La busca|Catedras\r"
expect "Record with BookID=12345 has been added to the database"

# add third book
send  "add 12347|978-2-12345086-3|la busca|catedra\r"
expect "Record with BookID=12347 has been added to the database"

# print books
send "printInd\n"
expect "Entry #0"
expect "    key: #12345"
expect "    offset: #80"
expect "    size: #37"
expect "Entry #1"
expect "    key: #12346"
expect "    offset: #0"
expect "    size: #72"
expect "Entry #2"
expect "    key: #12347"
expect "    offset: #125"
expect "    size: #36"

# delete 12345 and 12347
send "del 12346\n"
expect "Record with BookID=12346 has been deleted"
send "del 12345\n"
expect "Record with BookID=12345 has been deleted"

# print deleted
send "printLst\n"
expect "Entry #0"
expect "    offset: #0"
expect "    size: #72"
expect "Entry #1"
expect "    offset: #80"
expect "    size: #37"

# add 12348
send  "add 12348|978-2-12345086-3|La busca|Catedra\r"
expect "Record with BookID=12348 has been added to the database"

# add 12349
send  "add 12349|978-2-12345086-3|la busca|catedras\r"
expect "Record with BookID=12349 has been added to the database"

# print books
send "printInd\n"
expect "Entry #0"
expect "    key: #12347"
expect "    offset: #125"
expect "    size: #36"
expect "Entry #1"
expect "    key: #12348"
expect "    offset: #0"
expect "    size: #36"
expect "Entry #2"
expect "    key: #12349"
expect "    offset: #80"
expect "    size: #37"

# print deleted
send "printLst\n"
expect "Entry #0"
expect "    offset: #44"
expect "    size: #28"

send "exit\n"
expect "all done"

puts  "1) Reuse deleted book space OK, ;-)"
puts "2) Script end"