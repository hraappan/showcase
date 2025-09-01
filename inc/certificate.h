#pragma once

#include "tls-connection.h"


int CA_certificate_file(TLSConnection **tls);
int CA_certificate_priv_file(TLSConnection **tls);