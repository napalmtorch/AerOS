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
            void ParseImgTag();
            char* local_data;
        private:
            bool IsInBody();
            bool ContainsJavaScript();
            bool HasCSS();
            void ParseInlineCSS();
            void ParseHrefCSS();
            ~Parser();
    };
}