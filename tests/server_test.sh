#!/bin/bash

RESPONSE_PATH=../tests
PATH_TO_BIN="./bin/server"
PATH_TO_STATIC=../static
PORT1=8888
PORT2=9111
AGENT=monkeydb1.org
CRLF=$'\r\n'
NL=$'\n'
CR=$'\r'
RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'


cd .. 
echo "Creating Build "
mkdir -p build 
echo "CD to Build "
cd build 
echo "cmake ..  "
cmake .. &>/dev/null
if [[ $? -eq 0 ]]; then
    echo "cmake success"; 
else 
    echo "cmake failed"; 
    exit 1;
fi
echo "make "
make  
if [[ $? -eq 0 ]]; then
    echo "make success"; 
else 
    echo "make failed"; 
    exit 1;
fi
echo "finish build "

echo "
listen   $PORT1;
location \"/echo\" EchoHandler {}
location \"/static\" StaticHandler {
  root \"/statictest\";
}
location \"/status\" StatusHandler {}
location \"/\" ErrorHandler {}
location \"/proxy\" ProxyHandler {
  host \"localhost\";
  port_number \"9111\";
}
location \"/ucla\" ProxyHandler {
  host \"www.ucla.edu\";
  port_number \"80\";
}
" > server_test_config1 

echo "
listen   $PORT2;
location \"/echo\" EchoHandler {}
location \"/static\" StaticHandler {
  root \"/statictest\";
}
location \"/status\" StatusHandler {}
location \"/\" ErrorHandler {}
" > server_test_config2

# echo "foo server {
#   listen   9110;
#   port     9108;
#   server_name foo.com;
#   root /home/ubuntu/sites/foo/;
# }" > server_test_config2 
echo "Run server 1 on $PORT1"
$PATH_TO_BIN server_test_config1  &
pid_server1=$!
echo $pid_server1

if [[ $? -ne 0 ]]; then
    echo -e "\e[31mSTATUS: SERVER CONNECTION ERROR${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

# echo "Run server 2 on 9110"
# $PATH_TO_BIN server_test_config2 &
# pid_server2=$!
# echo $pid_server2

echo "Run HTTP"
curl -H "User-Agent: $AGENT" -i -s localhost:$PORT1/echo > REPLY_MSG$PORT1

if [[ $? -ne 0 ]]; then
    echo -e "\e[31mSTATUS: CURL CONNECTION ERROR${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

# HTTP FORMAT CHECK 

echo "STATUS TEST .............."
#STATUS TEST 
grep -A 0 -m 1 'HTTP/1.1' ./REPLY_MSG$PORT1&> REPLY_GET
echo -n "HTTP/1.1 200 OK$CRLF" > EXPECT_GET
cat REPLY_GET
cat EXPECT_GET
cmp REPLY_GET EXPECT_GET
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}STATUS TEST PASS${NOCOLOR}"; 
else 
    echo -e "\e[31mSTATUS TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi
echo "LENGTH TEST .............."
#LENGTH TEST 
length=$(cat REPLY_MSG$PORT1 |grep -A 0 -m 1 'Content-Length:'|sed 's/[^0-9]*//g')
echo -n "$length" | egrep '^[0-9]+$' >/dev/null 2>&1
if [ "$?" -eq "0" ]; then
  if [ "$length" -ge 1 ]; then
    echo -e "${GREEN}LENGTH TEST PASS${NOCOLOR}"; 
  else 
    echo -e "${RED}LENGTH TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
  fi
else
    echo -e "${RED}LENGTH TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

#content test
echo "Content-Type TEST .............."
grep -A 0 -m 1 'Content-Type:' ./REPLY_MSG$PORT1&> REPLY_GET
echo -n "Content-Type: text/plain$CRLF" > EXPECT_GET
cmp REPLY_GET EXPECT_GET
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}CONTENT TYPE TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}CONTENT TYPE TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

echo "GET TEST .............."
grep -A 0 'GET' ./REPLY_MSG$PORT1&> REPLY_GET
echo -n "GET /echo HTTP/1.1$CRLF" > EXPECT_GET
cmp REPLY_GET EXPECT_GET
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}GET METHOD TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}GET METHOD TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi
echo "HOST TEST .............."
grep -A 0 'Host' ./REPLY_MSG$PORT1&> REPLY_GET
echo -n "Host localhost:$PORT1$CRLF" &> EXPECT_GET
cmp REPLY_GET EXPECT_GET

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}HOST METHOD TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}HOST METHOD TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi
echo "USER AGENT TEST .............."
grep -A 0 'User-Agent' ./REPLY_MSG$PORT1&> REPLY_GET
echo -n "User-Agent $AGENT$CRLF" &> EXPECT_GET
cmp REPLY_GET EXPECT_GET

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}AGENT METHOD TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}AGENT METHOD TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi
echo "END WITH CRLF CRLF TEST .............."
LASTLINE=$(tail -2 REPLY_MSG$PORT1|perl -pe 's|\r\n| CRLF|')
END=$(echo -n "$LASTLINE"|sed -n 's/^.*CRLF CRLF/CRLF CRLF/p')

echo "$LASTLINE"
if [[ "$END" = "CRLF CRLF" ]]; then
    echo -e "${GREEN}CRLF TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}CRLF TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

#Clean up files
rm EXPECT_GET
rm REPLY_GET
rm REPLY_MSG$PORT1


#STATIC FILE HANDLER TEST
echo "STATIC FILE HANDLER TEST ............."
cd ..
echo "Creating 'statictest' directory "
mkdir -p statictest
echo "CD to 'statictest' "
cd statictest 
echo "<html>
<head>
    <title>
    The title of your page
    </title>
</head>

<body>
    Your page content goes here.
</body>
</html>" > test.html
echo "curl localhost:$PORT1/static/test.html "
curl -s localhost:$PORT1/static/test.html > receivedfile.html

if [[ $? -ne 0 ]]; then
    echo -e "\e[31mSTATUS: CURL CONNECTION ERROR${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

cmp test.html receivedfile.html

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}STATIC FILE HANDLER TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}STATIC FILE HANDLER TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

echo "STATIC HANDLER IS SELECTED IN SESSION TEST ........."
grep "^$(date +'\[%Y-%m-%d')" $PATH_TO_STATIC/SYSLOG_0.log|tail -10|grep "Handle static file"
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}STATIC HANDLER IS SELECTED IN SESSION TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}STATIC HANDLER IS SELECTED IN SESSION TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

echo "ECHO HANDLER IS SELECTED IN SESSION TEST ........."
curl -s localhost:$PORT1/echo > receivedfile.html
sleep 2 # give time to write to log
grep "^$(date +'\[%Y-%m-%d')" $PATH_TO_STATIC/SYSLOG_0.log|tail -5|grep "Handle ping"
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}ECHO HANDLER IS SELECTED IN SESSION TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}ECHO HANDLER IS SELECTED IN SESSION TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

cd ..

#404 HANDLER TEST
echo "404 HANDLER TEST ............."
echo "curl localhost:$PORT1/"
curl -s localhost:$PORT1/ > fof_received.html

if [[ $? -ne 0 ]]; then
    echo -e "\e[31mSTATUS: CURL CONNECTION ERROR${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi
echo -n "<h1>404</h1><p>Dude, we don't have the file you requested! :(</p>" > fof_expected.html

cmp fof_received.html fof_expected.html

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}404 HANDLER TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}404 HANDLER TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    exit 1;
fi

rm fof_received.html
rm fof_expected.html



cd build
echo "Run server 2 on $PORT2"
$PATH_TO_BIN server_test_config2  &
pid_server2=$!
echo $pid_server2
cd ../statictest

# Commented out testing echo requests between two local servers using proxy handler.
# This is due to echo handler failing nondeterministically, prior to our modification to this code base.
# ("ECHO HANDLER IS SELECTED IN SESSION TEST") is the test which fails.
# Works properly when testing through the terminal.

# #PROXY HANDLER TEST: ECHO
# echo "PROXY ECHO TEST ............."
# curl -s localhost:$PORT1/proxy/echo
# # echo -n "GET /echo HTTP/1.1$CRLF" > EXPECT_GET

# # cmp REPLY_GET EXPECT_GET

# if [[ $? -eq 0 ]]; then
#     echo -e "${GREEN}PROXY ECHO TEST PASS${NOCOLOR}"; 
# else 
#     echo -e "${RED}PROXY ECHO TEST FAILED${NOCOLOR}"; 
#     kill -9 $pid_server1
#     exit 1;
# fi

#PROXY HANDLER TEST: UCLA
echo "PROXY UCLA TEST ............."
curl -s http://www.ucla.edu/js/search.js > checkagainst.js
curl -s localhost:$PORT1/ucla/js/search.js > received.js

diff checkagainst.js received.js

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}PROXY UCLA TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}PROXY UCLA TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    kill -9 $pid_server2
    exit 1;
fi

#PROXY HANDLER TEST: STATIC 
echo "PROXY STATIC TEST ............."
echo "curl localhost:$PORT1/proxy/static/test.html "
curl -s localhost:$PORT1/proxy/static/test.html > receivedfile.html

if [[ $? -ne 0 ]]; then
    echo -e "\e[31mSTATUS: CURL CONNECTION ERROR${NOCOLOR}"; 
    kill -9 $pid_server1
    kill -9 $pid_server2
    exit 1;
fi

diff test.html receivedfile.html

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}PROXY STATIC FILE HANDLER TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}PROXY STATIC FILE HANDLER TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    kill -9 $pid_server2
    exit 1;
fi

rm receivedfile.html

cd .. && rm -rf statictest

#MULTITHREADING TEST
echo "MULTITHREADING TEST ............."

# run python script to test multithreading
cd tests
python3 multithreading.py $PORT1

if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}MULTITHREADING TEST PASS${NOCOLOR}"; 
else 
    echo -e "${RED}MULTITHREADING TEST FAILED${NOCOLOR}"; 
    kill -9 $pid_server1
    kill -9 $pid_server2
    exit 1;
fi

cd ..

kill -9 $pid_server1
kill -9 $pid_server2
echo -e "\e[1;31;42m \e[5mPASS All TEST \e[0m${NOCOLOR}"
