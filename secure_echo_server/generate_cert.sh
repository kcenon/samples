#!/bin/bash

###############################################################################
# Certificate Generation Script for Secure Echo Server
#
# This script generates a self-signed SSL certificate for testing purposes.
# DO NOT use these certificates in production environments.
###############################################################################

set -e

echo "==================================================="
echo "  Generating Self-Signed SSL Certificate"
echo "==================================================="
echo ""

# Certificate details
CERT_FILE="server.crt"
KEY_FILE="server.key"
DAYS_VALID=365
COUNTRY="KR"
STATE="Gyeonggi"
CITY="Uijeongbu"
ORG="Sample Organization"
ORG_UNIT="Development"
COMMON_NAME="localhost"

echo "[INFO] Certificate will be valid for $DAYS_VALID days"
echo "[INFO] Common Name (CN): $COMMON_NAME"
echo ""

# Check if OpenSSL is installed
if ! command -v openssl &> /dev/null; then
    echo "[ERROR] OpenSSL is not installed"
    echo "[ERROR] Please install OpenSSL first:"
    echo "  macOS:   brew install openssl"
    echo "  Ubuntu:  sudo apt-get install openssl"
    exit 1
fi

# Remove existing certificates if present
if [ -f "$CERT_FILE" ] || [ -f "$KEY_FILE" ]; then
    echo "[WARN] Existing certificates found"
    read -p "Do you want to overwrite them? (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "[INFO] Certificate generation cancelled"
        exit 0
    fi
    rm -f "$CERT_FILE" "$KEY_FILE"
fi

# Generate private key and self-signed certificate
echo "[INFO] Generating RSA private key (2048 bit)..."
openssl req -x509 -newkey rsa:2048 -nodes \
    -keyout "$KEY_FILE" \
    -out "$CERT_FILE" \
    -days "$DAYS_VALID" \
    -subj "/C=$COUNTRY/ST=$STATE/L=$CITY/O=$ORG/OU=$ORG_UNIT/CN=$COMMON_NAME" \
    2>/dev/null

if [ $? -eq 0 ]; then
    echo ""
    echo "==================================================="
    echo "  Certificate Generation Complete"
    echo "==================================================="
    echo ""
    echo "Files created:"
    echo "  Certificate: $CERT_FILE"
    echo "  Private Key: $KEY_FILE"
    echo ""
    echo "Certificate details:"
    openssl x509 -in "$CERT_FILE" -text -noout | grep -A 2 "Subject:"
    echo ""
    echo "NOTE: This is a self-signed certificate for testing only."
    echo "      Clients will need to accept or ignore certificate warnings."
    echo ""
else
    echo "[ERROR] Certificate generation failed"
    exit 1
fi
