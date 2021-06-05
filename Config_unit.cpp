#include "Config_unit.hpp"

location_unit &location_unit::operator=(location_unit const &cp) {
    _abs_path = cp._abs_path;
    _methods = cp._methods;
    _autoindex = cp._autoindex;
    _def_file = cp._def_file;
    return (*this);
}

listen_unit &listen_unit::operator=(listen_unit const &for_copy) {
    str = for_copy.str;
    for (int i = 0; i < 4; i++)
        digit[i] = for_copy.digit[i];
    type = for_copy.type;
    port = for_copy.port;
    return (*this);
}

void Config_unit::setDefaultFile(std::string const &str) {
    _def_file = str;
}

Config_unit &Config_unit::operator=(Config_unit const &for_copy) {
    _name = for_copy._name;
    _location = for_copy._location;
    _listen = for_copy._listen;
    _methods = for_copy._methods;
    _error = for_copy._error;
    _cgi_loc = for_copy._cgi_loc;
    _max_client_body = for_copy._max_client_body;
    _err_location = for_copy._err_location;
    _autoindex = for_copy._autoindex;
    return (*this);
}

std::list<std::string> &Config_unit::setName(void) {
    return (_name);
};

std::map<std::string, location_unit> &Config_unit::setLocation(void) {
    return (_location);
};

std::list<listen_unit> &Config_unit::setListen(void) {
    return (_listen);
};

std::list<std::string> &Config_unit::setMethods(void) {
    return (_methods);
};

int &Config_unit::setError(void) {
    return (_error);
};

std::string &Config_unit::setCGI_loc(void) {
    return (_cgi_loc);
};

unsigned int &Config_unit::setMax_client_body(void) {
    return (_max_client_body);
};

std::map<int, std::string> &Config_unit::setErr_location(void) {
    return (_err_location);
};

int &Config_unit::setAutoindex(void) {
    return (_autoindex);
};

void Config_unit::setWorkers(int number) {
    _workers = number;
};

std::list<std::string> const &Config_unit::getName(void) const {
    return (_name);
};

std::map<std::string, location_unit> const &Config_unit::getLocation(void) const {
    return (_location);
};

std::list<listen_unit> const &Config_unit::getListen(void) const {
    return (_listen);
};

std::list<std::string> const &Config_unit::getMethods(void) const {
    return (_methods);
};

int Config_unit::getError(void) const {
    return (_error);
};

std::string const &Config_unit::getCGI_loc(void) const {
    return (_cgi_loc);
};

unsigned int Config_unit::getMax_client_body(void) const {
    return (_max_client_body);
};

std::map<int, std::string> const &Config_unit::getErr_location(void) const {
    return (_err_location);
};

int Config_unit::getAutoindex(void) const {
    return (_autoindex);
};

int Config_unit::getWorkers(void) const {
    return (_workers);
}

std::string const &Config_unit::getDefaultFile(void) {
    return (_def_file);
};

Config_unit::Config_unit(void) {
    _autoindex = 0;
    _error = 0;
    _max_client_body = -1;
    _workers = 1;
};

Config_unit::~Config_unit(void) {};

int Config_unit::checkAutoindex(std::string const &path) {
    return (_getLocation(path)->second._autoindex);
}

/*
принимает номер ошибки, отдаёт асолютный путь к файлу с ошибкой
если такого нет - отдаёт генерирует путь [корень/сайта/номер_ошибки.html]
*/
std::string const Config_unit::searchError_page(int err_num) {
    std::map<int, std::string>::iterator it;

    it = _err_location.find(err_num);
    if (it != _err_location.end())
        return (it->second);
    std::stringstream temp;
    temp << err_num;
    return (_err_location.begin()->second + temp.str() + ".html");
};

/*
checkMethod(метод, путь_в_запросе(неизменённый, как в первой строке))
функция отвечает возможено ли применить такой метод к такому запросу
*/
bool Config_unit::checkMethod(std::string const &method, \
            std::string const &path) {
    std::list<std::string>::iterator it;
    std::list<std::string>::iterator end;
    std::map<std::string, location_unit>::iterator loc;

    loc = _getLocation(path);
    it = loc->second._methods.begin();
    end = loc->second._methods.end();
    while (it != end) {
        if (*it == method)
            return true;
        ++it;
    }
    return (false);
}

void Config_unit::resort(void) {
    _name.sort();
    _methods.sort();
}

std::string Config_unit::getServerPath(std::string const &path) {
    int cnt;
    std::string res;
    std::map<std::string, location_unit>::iterator it = _getLocation(path);

    res = it->second._abs_path;
    cnt = it->first.size();
    if (path[0] == '/' && res[res.size() - 1] == '/')
        ++cnt;
    if (res[res.size() - 1] != '/')
        res.append("/");
    res.append(&path[cnt]);
    if (res[res.size() - 1] == '/')
        res.append(it->second._def_file);
    return res;
}

std::map<std::string, location_unit>::iterator\
 Config_unit::_getLocation(std::string const &path) {
    std::map<std::string, location_unit>::iterator it;
    std::map<std::string, location_unit>::iterator end;

    it = _location.begin();
    end = _location.end();
    ++it;
    while (it != end) {
        if (it->first.find(path.c_str(), 0, it->first.size()) == 0)
            return it;
        ++it;
    }
    return _location.begin();
}
