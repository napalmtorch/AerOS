#include <software/security.hpp>
#include <lib/types.h>
#include <lib/string.hpp>
namespace AerOS
{
bool Permissions::IsGuest(char* name)
{
     if(streql(name,"guest")) { return true; } else { return false; }
}
bool Permissions::IsUser(char* name)
{
    if(streql(name,"aeros")) { return true; } else { return false; }
}
bool Permissions::IsRoot(char* name)
{
    if(streql(name,"root")) { return true; } else { return false; }
}
}