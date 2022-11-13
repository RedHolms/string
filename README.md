Here's little stupid implimentation of encoded strings  
String receives an encoding as template parameter  
Each encoding also have a char table  
Example of encoding:  
	&emsp;UTF-8 - encoding (char_type = char or char8_t)  
	&emsp;UNICODE - char table of UTF-8 (max_char_type = char32_t)  

You can probably add other encodings to this  

WIP
