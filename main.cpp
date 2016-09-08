#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <algorithm>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <curl/curl.h>

using namespace std;

string data;

string ParseHTML(string &expression, string &searchString);
int urlOpen(string URL);
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
bool vectorContainsElement(vector<int> &listOfNums, int num);
void listDir(vector<string> &filesInDirectory);
string get_file_contents(const char *filename);

int main()
{
	srand (time(NULL));
	
	vector<string> FileVector;
	vector<string> ListOfIDNumbers;

	vector<string> VecotrofURLs;
	
	vector<string> FullZipCodes;
	
	string filename_toOpen = "EntireTrimmedData.txt";
	string filename_toWrite = "FinalDoneData.txt";

	string filename_toOpen_two = "trimmedFelon2.txt";
	string filename_toOpen_three = "trimmedFelon3.txt";
	
	//string websitePartOne = "https://tools.usps.com/go/ZipLookupResultsAction!input.action?resultMode=1&companyName=&address1=";
	//string websitePartTwo = "&address2=&city=&state=&urbanCode=&postalCode=&zip=";
	
	string websitePartOne = "www.mapquest.com/search/results?page=0&centerOnResults=1&query=";
	//string websitePartTwo = "";
	
	//http://www.mapquest.com/search/results?page=0&centerOnResults=1&query=854B Orienta Ave. 32701
	
	
	
	//string Regex_Expression_String = "style=\"\">(.*?)</span><span .*?class=\"zip4\">(.*?)</span>";
	string Regex_Expression_String = " (\\d{5}-\\d{4})";
	
	
	//get cwd 
	char* cwd;
	char buffer[PATH_MAX];
	cwd = getcwd(buffer, PATH_MAX);
	string currentDirectory(cwd);
	int chdirRet;
	int mkdirRet;
	
	//create directory strings
	string masterDirectory = currentDirectory + "/MapQuestDataFiles";
	
	//convert directory strings to const char for use in mkdir
	const char* cCurrentDirectory = currentDirectory.c_str();
	const char* cMasterDirectory = masterDirectory.c_str();
	
	//Create HTML cache directories
	mkdirRet = mkdir(cMasterDirectory, S_IRWXU | S_IRWXG | S_IRWXO);
	if (mkdirRet == -1)
		cout << "\"" << cMasterDirectory << "\" already exists!" << endl;
	
	
	
	ifstream CSVFile;
	CSVFile.open(filename_toOpen);
	
	if (CSVFile.is_open()){
		
		string line = "";
		
		while (CSVFile.good())
		{
			getline (CSVFile,line);
			FileVector.push_back(line);
		}
		CSVFile.close();
		
	}
	else{
		cout << "Unable to open file!" << endl;
		return -1;
	}
	
	if(FileVector[FileVector.size()-1] == ""){
		FileVector.pop_back();
	}
	
	for (size_t i = 0; i < FileVector.size(); i++) {

		string TempString = FileVector[i];
		vector<string> TempVector;
		stringstream TempSS(TempString);
		string k = "";
		string IDNumber = "";
		
		TempSS >> IDNumber;
		ListOfIDNumbers.push_back(IDNumber);

		while (TempSS >> k) {
			TempVector.push_back(k);
		}

		k = "";

		for (size_t j = 0; j < TempVector.size() - 1; j++) {
			if (j == TempVector.size() - 2) {
				k = k + TempVector[j];
			}
			else {
				k = k + TempVector[j] + "+";
			}
			
		}

		k = websitePartOne + k + "+" + TempVector[TempVector.size() - 1];

		VecotrofURLs.push_back(k);

	}
	
	for (size_t m = 0; m < VecotrofURLs.size(); m++){
		
		size_t pos = 0;
		
		pos = VecotrofURLs[m].find("#");
		
		if(pos != string::npos){
			VecotrofURLs[m].replace(pos, 1, "");
		}
		
	}
	
	// generate 12000 (unique) random numbers between 0 and 225,737
	/*
	vector<int> vectorOfChosenNums;
	bool validNum = false;
	int randNum;
	
	for (int i = 0; i < 12000; ++i)
	{
		while(validNum == false)
		{
			randNum = rand() % 225738;
			
			if(vectorContainsElement(vectorOfChosenNums, randNum))
			{
				//if vector has element we want to stay in while loop
			}
			
			else
			{
				validNum = true;
			}
		}
		
		validNum = false;
		vectorOfChosenNums.push_back(randNum);
	}
	*/
	//sort(vectorOfChosenNums.begin(), vectorOfChosenNums.end());
	
	
	for(size_t i = 0; i < VecotrofURLs.size(); i++){
		
		cout << i+1 << endl;
		int urlOpenRet = 0;
		string StringMatch;
		
		chdirRet = chdir(cMasterDirectory);
		if (chdirRet == -1)
		{
			cout << "Could not change directory! Quitting." << endl;
			return 1;
		}
		
		vector<string> filesInDirectory;
		listDir(filesInDirectory);
		
		if(find(filesInDirectory.begin(), filesInDirectory.end(), ListOfIDNumbers[i] + ".html") != filesInDirectory.end())
		{
			data = "";
			data = get_file_contents((ListOfIDNumbers[i] + ".html").c_str());
			
		}
		
		else{
			sleep(2);
			
			for (int k = 0; k < 5; k++)
			{
				
				data = "";
				urlOpenRet = urlOpen(VecotrofURLs[i]);
				
				//if successful break out of the loop
				if (urlOpenRet == 0)
				{
					break;
				}
				sleep(15);
			}
			
			ofstream HTMLFileOutput;
		
			HTMLFileOutput.open (ListOfIDNumbers[i] + ".html");
			HTMLFileOutput << data << endl;
			HTMLFileOutput.close();	
			
		}
		
		chdirRet = chdir(cCurrentDirectory);
		if (chdirRet == -1)
		{
			cout << "Could not change directory! Quitting." << endl;
			return 1;
		}
		
		//PARSE
		StringMatch = ParseHTML(Regex_Expression_String, data);
		
		/*
		if(CompleteMatches[0] == ""){
			FullZipCodes.push_back("");
		}
		else{
			FullZipCodes.push_back(CompleteMatches[0] + "-" + CompleteMatches[1]);
		}
		*/
		
		//FullZipCodes.push_back(StringMatch);
		
		//cout << ListOfIDNumbers[i] << "\t" << StringMatch << endl;
		
		fstream fileInput;
		fileInput.open ("EntireCompleteDataFile.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		fileInput << ListOfIDNumbers[i] << "\t" << StringMatch << endl;
		fileInput.close();	
		
		//Put into FullZipCodes (MAKE SURE TO CHECK EMPTY VECTOR FROM PARSE)
		//Clear Data
		
	}
	
	/*
	cout << "Going to Full Data Now!" << endl;
	
	
	for(size_t i=0; i < VecotrofURLs.size(); i++){
	//Loop VecotrofURLs.size Times
		//Call URLOpen
		cout << i+1 << endl;
		int urlOpenRet = 0;
		string StringMatch;
		
		/*
		if(i % 10 == 9)
		{
			sleep(10);
		}
		END COMMENT HERE
		//sleep(10);
		
		//try to open the link 5 times
		for (int k = 0; k < 5; k++)
		{
			
			data = "";
			urlOpenRet = urlOpen(VecotrofURLs[i]);
			
			//if successful break out of the loop
			if (urlOpenRet == 0)
			{
				break;
			}
			sleep(15);
		}
		
		//if after 5 tries we still fail quit the program
		/*
		if (urlOpenRet == -1)
		{
			CompleteMatches.push_back("N/A");
			CompleteMatches.push_back("N/A");
		}
		END COMMENT HERE
		
		chdirRet = chdir(cMasterDirectory);
		if (chdirRet == -1)
		{
			cout << "Could not change directory! Quitting." << endl;
			return 1;
		}
		
		ofstream HTMLFileOutput;
		
		ostringstream convert;
		convert << i+1;
		string Result = convert.str();
		
		HTMLFileOutput.open (Result + ".html");
		HTMLFileOutput << data << endl;
		HTMLFileOutput.close();	
		
		chdirRet = chdir(cCurrentDirectory);
		if (chdirRet == -1)
		{
			cout << "Could not change directory! Quitting." << endl;
			return 1;
		}
		
		//PARSE
		StringMatch = ParseHTML(Regex_Expression_String, data);
		
		/*
		if(CompleteMatches[0] == ""){
			FullZipCodes.push_back("");
		}
		else{
			FullZipCodes.push_back(CompleteMatches[0] + "-" + CompleteMatches[1]);
		}
		END COMMENT HERE
		
		FullZipCodes.push_back(StringMatch);
		
		fstream fileInput;
		fileInput.open ("completeDataFile.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		fileInput << ListOfIDNumbers[i] << StringMatch << endl;
		fileInput.close();	
		
		//Put into FullZipCodes (MAKE SURE TO CHECK EMPTY VECTOR FROM PARSE)
		//Clear Data
	}
	*/
	
	//Combine FileVector and FullZipCodes
	//Write Data to File 
	
	
	
	return 0;
}

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{	//callback must have this declaration
	//buf is a pointer to the data that curl has for us
	//size*nmemb is the size of the buffer
	
	for (size_t c = 0; c < size*nmemb; c++)
	{
		data.push_back(buf[c]);
	}
	return size*nmemb; //tell curl how many bytes we handled
}

int urlOpen(string URL)
{
	CURL *curl;
	CURLcode res;
	string userAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.1";
	
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
		// example.com is redirected, so we tell libcurl to follow redirection
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		
		//Perform the request, res will get the return code 
		res = curl_easy_perform(curl);
		//Check for errors  
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			
			return -1;
		}
		
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	
	return 0;
}

string ParseHTML(string &expression, string &searchString)
{
	const regex NCapRegex(expression);
	smatch matches;
	string SingleMatch;
	/*
	fstream fileInput;
	fileInput.open ("HTML_File.html");
	fileInput << searchString << endl;
	fileInput.close();
	*/
	try
	{
		if(regex_search(searchString, matches, NCapRegex)){
			SingleMatch = matches[1];
			
			if(matches.size() > 2){
				SingleMatch = SingleMatch + "\tMultiple Matches Found, This is the top result.";
			}
			
		}
		else{
			SingleMatch = "";
		}
	}
	
	//if an error occurs exit program
	catch(regex_error& e)
	{
		cout << "A regex error has occured" << endl;
		exit (1);
	}
	
	return SingleMatch;
}

bool vectorContainsElement(vector<int> &listOfNums, int num)
{
	if(find(listOfNums.begin(), listOfNums.end(), num) != listOfNums.end())
	{
		return true;
	}
	
	else
	{
		return false;
	}
}

void listDir(vector<string> &filesInDirectory)
{
	filesInDirectory.clear();
	
	DIR		*d;
	struct dirent *dir;
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			filesInDirectory.push_back(dir->d_name);
		}
		
		closedir(d);
	}
}

string get_file_contents(const char *filename)
{
	ifstream in(filename, ios::in);
	if (in)
	{
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw(errno);
}