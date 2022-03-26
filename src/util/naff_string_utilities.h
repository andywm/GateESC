/*------------------------------------------------------------------------------
 ()		File: naff_string_utilities.h
 /\		Authour: Andrew Woodward-May
/  \	Date: March 2022	License: MIT

Description:
	Some basic string utilities to make formatting for a limited display output
	a little easier.
------------------------------------------------------------------------------*/
#pragma once
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <string.h>
#include "printf.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// NaffSPrintfAutoFormat
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct NaffSPrintfAutoFormat
{
	/// Provide sprintf with formatting option for unknown type, provides string
	template<typename T>
	static const char* ResolveOutput(T&t)
	{
		return "%s";
	}
};


class NaffStringUtility
{

public:
	/// Very basic formatting system. Will replace #####s in your string
	/// with the arguments provided.
	/// Input and output buffers must be of the same size. This is not checked.
	template<typename ...PackedArgs>
	static void Format(char Output[], const char* Input, PackedArgs&&... Args)
	{
		//copy buffer verbatim.
		memcpy(Output, Input, 20);
		
		//if we have vargs then try and replace hashes with the arguments.
		if( sizeof...(Args) > 0 )
		{
			FormatReplace( Output, Input, Args... );
		}
	}

private:
	/// Finds first concurrent set of hashes (i.e. #) in a string, increments
	/// OutputBuffer to the begining of this sequence, and InputBuffer
	/// to the end. Returns how many characters are present in the block
	/// of hash characters.
	static int TryGetReplace(char*& OutputBuffer, const char*& InputBuffer )
	{
		int replacecount = 0;
		
		do
		{
			if( *InputBuffer == '#')
			{
				++replacecount;
				
				if( *(InputBuffer+1) != '#' )
				{
					break;
				}
			}		
			
			InputBuffer++;
			
			if( replacecount == 0 )
			{
				OutputBuffer++;
			}
		} while (*InputBuffer != '\0');
		
		return replacecount;
	}

	/// No-op overload for fully unpacked.
	template<typename ...PackedArgs>
	static void FormatReplace(char*& Output, const char*& Input )
	{  
	}

	template<typename InputType, typename ...PackedArgs>
	static void FormatReplace(char*& Output, const char*& Input, InputType Param, PackedArgs... Args)
	{     
		int replaceLen = TryGetReplace(Output, Input);
		if( replaceLen > 0 )
		{     
			//format
			char buffer[8];
			int copyLen = snprintf(&buffer[0], 8, NaffSPrintfAutoFormat::ResolveOutput(Param), Param);
			copyLen = copyLen > replaceLen ? replaceLen : copyLen;
			
			//clear the hashes in the destination buffer.
			memset(&Output[0], ' ', replaceLen);
			
			//copy stringified numeric value to destination buffer.
			strncpy(Output,buffer,copyLen);
			
			Output+=replaceLen;
			Input+=1;
		}
	
		//next...
		FormatReplace( Output, Input, Args...);
	}
};