#include "language.h"
#include <moFileReader.hpp>
#include "../DMI/dmi.h"
#include "../STM/stm.h"
#include "../Packets/STM/30.h"
#include <iostream>
moFileLib::moFileReader reader;
std::string language = "en";
std::string get_text(std::string id)
{
    if (language == "en") return id;
    return reader.Lookup(id.c_str());
}
std::string get_text_context(std::string context, std::string id)
{
    if (language == "en") return id;
    return reader.LookupWithContext(context.c_str(), id.c_str());
}
void set_language(std::string lang)
{
    std::string file = "../locales/evc/"+lang+".mo";
#ifdef __ANDROID__
    extern std::string filesDir;
    file = filesDir+"/locales/evc/"+lang+".mo";
#endif
    if (lang == "en") {
        language = "en";
    } else if (reader.ReadFile(file.c_str()) != moFileLib::moFileReader::EC_SUCCESS) {
        std::cout<<reader.GetErrorDescription()<<std::endl;
        language = "en";
    } else {
        language = lang;
    }
    for (auto it : installed_stms) {
        stm_message msg;
        auto *pack = new STMLanguage();
        pack->NID_DRV_LANGUAGE.rawdata = ((language[0]&0xFF)<<8)|(language[1]&0xFF);
        msg.packets.push_back(std::shared_ptr<ETCS_packet>(pack));
        it.second->send_message(&msg);
    }
    send_command("language", lang);
}