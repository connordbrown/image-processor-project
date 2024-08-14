# image-processor-project

## introduction

This project is a C++ adaptation of some of the the Runestone Interactive image processing projects. It takes in a .bmp image, changes it
in some way, and then outputs a new, altered version of the origianl .bmp image. I wrote the image processing, helper, and main functions in Brown_main.cpp. The file processing functions, as well as the starter files, were provided.

## operation

This application uses C++ 11 or later. Compile it with the following command:

```bash
g++ -std=c++11 Brown_main.cpp -o main
```

Then, run it:
```bash
./main
```

## sample output
```text
Image Processing Application

Enter input .bmp filename: sample.bmp 

IMAGE PROCESSING MENU
 0) Change image (current: sample.bmp)
 1) Vignette
 2) Clarendon
 3) Grayscale
 4) Rotate 90 degrees
 5) Rotate multiple 90 degrees
 6) Enlarge
 7) High contrast
 8) Lighten
 9) Darken
10) Black, white, red, green, blue

Enter menu selection (Q to quit): 1

Vignette selected
Enter output .bmp filename: output.bmp
Successfully applied vignette!

IMAGE PROCESSING MENU
 0) Change image (current: sample.bmp)
 1) Vignette
 2) Clarendon
 3) Grayscale
 4) Rotate 90 degrees
 5) Rotate multiple 90 degrees
 6) Enlarge
 7) High contrast
 8) Lighten
 9) Darken
10) Black, white, red, green, blue

Enter menu selection (Q to quit): Q

Goodbye.
```
## image samples: before and after

![alt text](https://github.com/connordbrown/image-processor-project/blob/main/sample.bmp "Original Image")

![alt text](https://github.com/connordbrown/image-processor-project/blob/main/output.bmp "Altered Image")

## author

This C++ adaptation was written by Connor D. Brown in 2024.