#pragma once
namespace Web
{
    class Parser
    {
        public:
            Parser(char* url);
            void CheckDoctype();
            void CheckForTitle();
            void CheckH1Tag();
            void RawDump();
            char* local_data;
        private:
            ~Parser();
    };
}