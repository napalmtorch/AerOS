#include <software/html_parse.hpp>
#include <lib/types.h>
#include <lib/string.hpp>
#include <core/kernel.hpp>
#include <hardware/nfs.hpp>
#include <hardware/memory.hpp>
namespace Web
{
    Parser::Parser(char* url)
    {
        if(startswith(url,"https://")) { debug_writeln("https currently not supported"); this->local_data = "https is currently not supported!";}
        else if(startswith(url,"http://")) { debug_writeln("http to be implemented"); this->local_data = "http will be implemented very soon together with the network driver"; }
        else if(startswith(url,"file:///")) { //Reading from filesystem, which requires three slash as normally only 2
            debug_writeln("Parsing local file"); 
        HAL::nfs_file_t local_url = System::KernelIO::NapalmFS.ReadFile(url);  
        char* raw_data = (char*)mem_alloc(sizeof(local_url.data)+1024); //until we have a reliable way to determine size lets just append to sizeof
        mem_copy((uint8_t*)local_url.data,(uint8_t*)raw_data,sizeof(local_url.data)+1024);
        this->local_data = raw_data;
        }
        else {
            debug_writeln_ext("Unsupported Protocol: ",COL4_RED);
            debug_write("Got: ");
            debug_writeln(url);
            this->local_data = "Unsupported Protocol!";
        }
    }

    void Parser::CheckDoctype()
    {
    if(StringContains(local_data,"<!DOCTYPE html>"))
    {
        System::KernelIO::Terminal.WriteLine("Html Document is HTML5",COL4_CYAN);
    }
    else
    {
        System::KernelIO::Terminal.WriteLine("Currently unknown HTML Header");
    }
    }

    void Parser::CheckForTitle() 
    {
    if(IsInBody()) {
    char *title1 = "<title>";
    char *title2 = "</title>";

    char *target = NULL;
    char *start, *end;

    if ( start = strstr( local_data, title1 ) )
    {
        start += strlen( title1 );
        if ( end = strstr( start, title2 ) )
        {
            target = ( char * )mem_alloc( end - start + 1 );
            mem_copy((uint8_t*)start,(uint8_t*)target, end - start );
            target[end - start] = '\0';
        }
    }

    if ( target ) { System::KernelIO::Terminal.Write("Title is: ",COL4_MAGENTA); System::KernelIO::Terminal.WriteLine(target,COL4_MAGENTA); }

    mem_free( target );
    }
    }
    void Parser::CheckH1Tag()
    {
    if(IsInBody()) {
    char *h1_1 = "<h1>";
    char *h1_2 = "</h1>";

    char *target = NULL;
    char *start, *end;

    if ( start = strstr( local_data, h1_1 ) )
    {
        start += strlen( h1_1 );
        if ( end = strstr( start, h1_2 ) )
        {
            target = ( char * )mem_alloc( end - start + 1 );
            mem_copy((uint8_t*)start,(uint8_t*)target, end - start );
            target[end - start] = '\0';
        }
    }

    if ( target ) { System::KernelIO::Terminal.Write("h1 content is: ",COL4_YELLOW); System::KernelIO::Terminal.WriteLine(target,COL4_YELLOW); }

    mem_free( target );
    }
    }

    bool Parser::IsInBody()
    {
    if(StringContains(local_data,"<body>") && StringContains(local_data,"</body>")) {
        return true;
    }
        return false;
    }

    bool Parser::ContainsJavaScript()
    {
        if(StringContains(local_data,"<script")) { return true; }
        return false;
    }

    void Parser::RawDump()
    {
        System::KernelIO::Terminal.Write("Raw HTML Output: ",COL4_MAGENTA);
        System::KernelIO::Terminal.WriteLine(local_data,COL4_CYAN);
    }
    Parser::~Parser()
    {
        mem_free(local_data);
    }
}