#pragma once

int doFile(const char *fileName);

int parseAlg(const char *param, const unsigned int pSize);

int parseWidth(const char *param, const unsigned int pSize);

int parseOption(const char *param, const unsigned int pSize);

void parseParameter(const char *param);

void parseCommandLine(const int argc, char* argv[]);