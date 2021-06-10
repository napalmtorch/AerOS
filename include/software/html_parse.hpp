#pragma once
namespace Web
{
    class Parser
    {
        public:
            Parser(char* url);
            void CheckDoctype();
            void CheckForTitle();
            char* CheckForTitleString();
            void CheckH1Tag();
            char* CheckH1TagString();
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