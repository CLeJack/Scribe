#pragma once
#include "Head.h"

//debugging
void printColumn(fvec& arr, std::string title);

void printColumn(float val, std::string title);

void printRows(const fvec& arr, std::string title);

void printRows(const ivec& arr, std::string title);

void printRows(const std::vector<bool>& arr, std::string title);

void printMatrix(fmatrix& mat, std::string title, int id);

void printMatrix(cmatrix& mat, std::string title, int id);

void printMatrixReal(cmatrix& mat, std::string title, int id);

fvec importCsv(std::string filename, int vecsize);

void printLine(fvec& arr, std::string title);

void printLine(ivec& arr, std::string title);

void printElement(float val);
//
