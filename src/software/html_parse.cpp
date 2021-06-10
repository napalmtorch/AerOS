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
        HAL::nfs_file_t local_url = System::KernelIO::NapalmFS.ReadFile(url);   
        this->local_data = (char*)local_url.data;
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

    void Parser::RawDump()
    {
        System::KernelIO::Terminal.Write("Raw HTML Output: ",COL4_MAGENTA);
        System::KernelIO::Terminal.WriteLine(local_data,COL4_CYAN);
    }
}