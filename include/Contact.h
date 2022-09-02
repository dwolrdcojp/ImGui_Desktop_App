#pragma once
#include <iostream>
#include <string>

class Contact {
public:
  std::string m_first_name = "";
  std::string m_last_name  = "";
  std::string m_phone      = "";
  std::string m_email      = "";

  Contact();

  Contact(std::string first, std::string last, std::string phone, std::string email)
    : m_first_name(first), m_last_name(last), m_phone(phone), m_email(email) {}
};
