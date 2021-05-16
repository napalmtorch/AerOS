#include <lib/types.h>
#include <core/kernel.hpp>
namespace System
{
    namespace Security
    {
        class Sudo
        {
            public:
                char* user = "aeros\0";
                char* Document;
                char* sudoers = "/etc/sudoers";
                bool CheckSudo(char* user);
            private:
                char* ParseSudoers();
        };
    }
}