To run these tests:

0. Be sure your CS id is activated.  If it is not, you can go to the
   CS Help Desk or look here: 
   https://www.dal.ca/faculty/computerscience/current/getting_set_up.html


1. Log into the course unix server.  
     csci3120.cs.dal.ca (the CSCI 3120 Unix VM)

   You will need to use an ssh client such as ssh (Mac)  or 
   Putty (Windows).

   Note: You will need your CS id and password to log into these servers.

   If you do not know how to do this, there are plenty of tutorials on
   the web on how to ssh into a unix server.

2. Create a csci3120 directory and an assn1 subdirectory on the unix server

      mkdir -p csci3120/assn1

3. Change directories into assn1

      cd csci3120/assn1

4. Copy (upload) the tests_1.zip file to csci3120/assn1 on the unix 
   server.

   Again, if you do not know how to do this, there are plenty of tutorials 
   on the web on how to copy files to a unix server.

5. On the unix server, unzip the zip file.

     unzip tests_1.zip

6a. Run the script file to run the tests.

     ./test.sh

6b. If a test fails, you have see the differences in output by running in
    debug mode.

     ./test.sh debug

Notes:
a.  If you are using a Mac, you have a unix system already.  You run 
    Terminal.app and do everything locally instead of using a remote
    unix server.

b.  You can run the tests inside your IDE, but you will need to copy
    and paste the input file into the console and then visually inspect
    the output.  This is workable for the short small tests, but is
    less workable for the larger ones. 
