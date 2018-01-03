//
//  main.cpp
//  illumio-test
//
//  Created by Ari Sweedler on 1/2/18.
//  Copyright © 2018 Ari Sweedler. All rights reserved.
//

#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

//open an OUTPUT file.
//open the FLOW file and read it one line at a time, adding to OUTPUT as we go

/************class IP address**************/
class IP_Address {
    short port;
    char ip[4];
    bool ipStar = false;
    bool portStar = false;
public:
    IP_Address();
    IP_Address(string myLine);
    IP_Address(const IP_Address&);
    long hash();
};

IP_Address::IP_Address() {
    for(int i = 0; i < 4; i++) {this->ip[i] = 0;}
    this->port = 0;
}

IP_Address::IP_Address(string str)
{
    //reads in a string, and sets ip[0-3] and port accordingly.
    char c;
    int ip_i = 0;
    int str_i = 0;
    
    if (str.at(0) == '*') {
        this->ipStar = true;
        str_i++;
    }
    for(; str_i < 16; str_i++) {
        c = str.at(str_i);
        if(c == '.') {
            ip_i++;
        } else if (c == ':') {
            break;
        } else {
            this->ip[ip_i] *= 10;
            this->ip[ip_i] += (c-'0');
        }
    }
    str_i++;
    if (str.at(str_i) == '*') {
        this->portStar = true;
    } else {
        this->port = std::atoi(str.substr(str_i).c_str());
    }
}

IP_Address::IP_Address(const IP_Address &ipa) {
    for(int i = 0; i < 4; i++) {
        this->ip[i] = ipa.ip[i];
    }
    this->port = ipa.port;
}

//we could get a better hash function here
long IP_Address::hash() {
    long answer = 0;
    for(int i = 0; i < 4; i++) {
        answer += this->ip[i];
        answer <<= 8;
    }
    answer += this->port;
    return answer;
}
/**************************/

/***************class NAT entry***********/
class NAT_Entry {
    IP_Address from;
    IP_Address to;
    NAT_Entry* next;
public:
    NAT_Entry();
    NAT_Entry(string);
    long hash();
};

NAT_Entry::NAT_Entry(string str)
{
    size_t commaIndex = str.find(",");
    this->from = IP_Address(str.substr(0, commaIndex));
    this->to = IP_Address(str.substr(commaIndex+1));
    this->next = nullptr;
}

long NAT_Entry::hash() {
    return this->from.hash();
}
/**************************/


/************MAIN**************/
NAT_Entry NAT_Database[256];
int myOpen (string name, ifstream* fileptr);
void addToOutput(string);

int main () {
    //open the NAT file and read in the data
    ifstream file;
    string myLine;
    myOpen("NAT", &file);
    while (getline(file, myLine)) {
        NAT_Entry myEntry = NAT_Entry(myLine);
        NAT_Database[myEntry.hash()] = myEntry;
    }
    myOpen("FLOW", &file);
    while (getline(file, myLine)) {
        addToOutput(myLine);//this checks our hashmap for out function and construcs an output accordingly.
    }
    
    return 0;
    // now we loop back and get the next line in 'str'
}

int myOpen (string name, ifstream* fileptr)
{
    fileptr->open(name);
    if (!(*fileptr)) {
        cerr << "Error: Unable to open file" << name << "." << endl;
        exit(1);
    }
    return 0;
}

void addToOutput(string myLine) {
    IP_Address myIP = IP_Address(myLine);//check hashmap and generate output accordingly
}
/**************************/
