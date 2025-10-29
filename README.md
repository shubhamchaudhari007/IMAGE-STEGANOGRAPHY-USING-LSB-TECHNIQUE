# 🖼️ Image Steganography using LSB Technique in C

### 🔐 Hide Your Secrets Inside an Image (24-bit BMP Only)

This project implements **Image Steganography** using the **Least Significant Bit (LSB)** technique — developed entirely in **C programming**.  
It allows users to **hide a text file inside a 24-bit BMP image** and later **extract it back** safely without visible distortion.

---

## 📘 Project Overview

Steganography is a powerful technique used to conceal secret information inside other non-secret media, like images or audio files.  
In this project, the data is embedded into the **Least Significant Bits** of the pixels in a BMP image — a modification invisible to the human eye but retrievable through decoding.

---

## 🧩 Supported Format

> ⚠️ **Important:**  
> This program supports **only 24-bit `.bmp` image files**.  
> 8-bit or 32-bit BMP files will not work properly because their pixel encoding structure differs.

---

## 🧠 Features

✅ **Encoding Mode**
- Hide any text-based secret file inside a `.bmp` image.  
- Image remains visually unchanged to the human eye.  
- Saves the output as a new stego image file.  

✅ **Decoding Mode**
- Retrieve and reconstruct the hidden file accurately.  
- Automatically restores the original extension of the secret file.  

✅ **Validation**
- Magic string ensures that decoding happens only on valid stego images.  
- Verifies file extensions and handles incorrect inputs gracefully.  

✅ **User-Friendly Terminal Output**
- Clean progress display with emojis and formatting for better readability.  

✅ **Lightweight and Cross-Platform**
- Runs on Linux, Windows (via MinGW), and macOS terminals.  

---

## 🗂️ File Structure

Image-Steganography/
│
├── main.c # Main program file (handles encode/decode selection)
├── encode.c / encode.h # Encoding logic implementation
├── decode.c / decode.h # Decoding logic implementation
├── types.h # Custom datatypes and enums (e.g., Status, OperationType)
├── common.h # Shared constants (like MAGIC_STRING)
└── README.md # Project documentation (this file)


---

## ⚙️ How It Works

### 🔹 Encoding (Hiding Data)
1. Reads a **24-bit BMP image**.  
2. Skips the **54-byte BMP header**.  
3. Encodes the following information bit by bit into the image’s LSBs:
   - A **magic string** (`#*`) for validation.  
   - The **secret file extension**.  
   - The **secret file size**.  
   - The **secret file content**.  
4. Saves the result as a **new stego image** (e.g., `stego.bmp`).

### 🔹 Decoding (Extracting Data)
1. Opens the **encoded stego BMP image**.  
2. Skips the header and reads the LSB bits.  
3. Validates the magic string.  
4. Extracts the **file extension**, **file size**, and **hidden content**.  
5. Reconstructs and saves the secret file (e.g., `Decode.txt`).

---

## 🧭 How to Run the Program

### 🔸 1. Compile the Project

If you’re using multiple `.c` files:
```bash
gcc main.c encode.c decode.c 

Encode Syntax : 
./a.out -e <input_image.bmp> <secret_file.txt> <output_stego.bmp>

./a.out -e flower.bmp message.txt stego.bmp

=============================================
🖼️  IMAGE STEGANOGRAPHY USING LSB TECHNIQUE
=============================================

🔐 ENCODING MODE SELECTED
---------------------------------------------
📂 Input BMP Image      : flower.bmp
📄 Secret Message File  : message.txt
💾 Output Stego Image   : stego.bmp
---------------------------------------------
⚙️  Encoding in progress...
✅ Magic String Embedded
✅ File Extension Saved
✅ File Size Encoded
✅ Secret Data Hidden Successfully
✨ Encoding Completed Successfully!
🎯 Stego Image Created: stego.bmp

your encoded file will be store in your repository as name stego.bmp

Decode Syntax : 

./a.out -d <encoded_stego.bmp> <output_file_name>

./a.out -d stego.bmp Decode.txt

=============================================
🔓  IMAGE STEGANOGRAPHY DECODING MODE
=============================================

📂 Input Stego Image : stego.bmp
---------------------------------------------
🔍 Validating Magic String...
✅ Verified Successfully
📦 Extracting File Extension...
📏 Extracting File Size...
💡 Decoding Hidden Data...
✅ Secret File Reconstructed: Decode.txt
---------------------------------------------
🎉 Decoding Completed Successfully!


your decoded file will be store in your repository as name Decode.bmp

© 2025 Shubham Chaudhari. All Rights Reserved.