#pragma once
namespace AerOS
{
    enum class Perms
    {
        None,
        User,
        All
    };
    class Permissions
    {
        public:
            bool IsUser(char* name);
            bool IsGuest(char* name);
            bool IsRoot(char* name);
            Perms curperms;
        private:
    };
}