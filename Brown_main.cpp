/*
main.cpp
CSPB 1300 Image Processing Application

PLEASE FILL OUT THIS SECTION PRIOR TO SUBMISSION

- Your name:
    Connor Brown

- All project requirements fully met? (YES or NO):
    YES

- If no, please explain what you could not get to work:
    N/A

- Did you do any optional enhancements? If so, please explain:
    NO - but did use <algorithm> header file for max() function
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @param bytes  the number of bytes to read
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset, int bytes)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < bytes; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2, 4);
    int start = get_int(stream, 10, 4);
    int width = get_int(stream, 18, 4);
    int height = get_int(stream, 22, 4);
    int bits_per_pixel = get_int(stream, 28, 2);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * (bits_per_pixel / 8);
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel> (width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // We are ignoring the alpha channel if there is one

            // Advance the position to the next pixel
            pos = pos + (bits_per_pixel / 8);
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>>& image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//


// ***** IMAGE PROCESSING FUNCTIONS ***** //


// adds vignette effect (dark corners) to image
vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
{
    // get image height (num_rows) and width (num columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original image RGB colors for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
                   
            // find distance to center of image from current pixel
            int distance = sqrt(pow(col - num_columns / 2, 2) + pow(row - num_rows / 2, 2));
            
            // get scaling factor - ensure that output is decimal value
            double scaling_factor = static_cast<double>(num_rows - distance) / num_rows;
            
            // get new color values by modifying original color values
            int new_red = red_value * scaling_factor;
            int new_green = green_value * scaling_factor;
            int new_blue = blue_value * scaling_factor;
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;  
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;
        }  
    }
    
    return new_image;
}


// adds Clarendon effect to image (darks darker and lights lighter) by a scaling_factor
vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
        
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB colors for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
            
            // get average color (gray) value of pixel
            double average_value = (red_value + blue_value + green_value) / 3.0;
            
            // if pixel is light, make a new one lighter
            int new_red = 0;
            int new_green = 0;
            int new_blue = 0;
            
            if (average_value >= 170)
            {
                new_red = 255 - (255 - red_value) * scaling_factor;
                new_green = 255 - (255 - green_value) * scaling_factor;
                new_blue = 255 - (255 - blue_value) * scaling_factor;
            }
            else if (average_value < 90)
            {
                new_red = red_value * scaling_factor;
                new_green = green_value * scaling_factor;
                new_blue = blue_value * scaling_factor;
            }
            else
            {
                new_red = red_value;
                new_green = green_value;
                new_blue = blue_value;
            }
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;     
        }  
    }
      
    return new_image;
}


// converts image to grayscale
vector<vector<Pixel>> process_3(const vector<vector<Pixel>>& image)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB colors for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
            
            // get average color (gray) value of pixel
            double gray_value = (red_value + green_value + blue_value) / 3.0;
            
            // set new color values to all be gray_value
            int new_red = gray_value;
            int new_green = gray_value;
            int new_blue = gray_value;
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;
        }
    }
    
    return new_image;
}


// rotates image clockwise by 90 degrees
vector<vector<Pixel>> process_4(const vector<vector<Pixel>>& image)
{
   
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with inverted dimensions of input image
    vector<vector<Pixel>> new_image(num_columns, vector<Pixel>(num_rows));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get current pixel of input image
            Pixel current_pixel = image.at(row).at(col);
            
            // map pixel to new image using inverted dimensions
            new_image.at(col).at(num_rows - 1 - row) = current_pixel;
        }
    }
    
    return new_image;
}


// rotates image clockwise by a specified number of multiples of 90 degrees
vector<vector<Pixel>> process_5(const vector<vector<Pixel>>& image, int number)
{
    int angle = number * 90;
    if (angle % 90 != 0)
    {
        cout << "Angle must be a multiple of 90 degrees." << endl;
    }
    else if (angle % 360 == 0)
    {
        return image;
    }
    else if (angle % 360 == 90)
    {
        return process_4(image);
    }
    else if (angle % 360 == 180)
    {
        return process_4(process_4(image));
    }
    
    // angle % 360 == 270
    return process_4(process_4(process_4(image)));
}


// enlarges images in x and y direction
vector<vector<Pixel>> process_6(const vector<vector<Pixel>>& image, int x_scale, int y_scale)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with height and width scaled according to x_scale and y_scale
    vector<vector<Pixel>> new_image(num_rows * y_scale, vector<Pixel>(num_columns * x_scale));
    
    // iterate through each pixel of scaled image
    for (int row = 0; row < num_rows * y_scale; ++row)
    {
        for (int col = 0; col < num_columns * x_scale; ++col)
        {
            // get current pixel of input image
            Pixel current_pixel = image.at(row / y_scale).at(col / x_scale);
            
            // map pixel to new_image using scaled dimensions
            new_image.at(row).at(col) = current_pixel;
        }
    }
    
    return new_image;
}


// converts image to high contrast (black and white only)
vector<vector<Pixel>> process_7(const vector<vector<Pixel>>& image)
{
    // get image height (num rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB colors for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;

            // get average color (gray) value of pixel
            double gray_value = (red_value + green_value + blue_value) / 3.0;

            // if pixel is light, make it white, else make it black
            int new_red = 0;
            int new_green = 0;
            int new_blue = 0;

            if (gray_value >= 255 / 2.0)
            {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            else
            {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;           
        }
    }
    
    return new_image;
}


// lightens image by a scaling_factor
vector<vector<Pixel>> process_8(const vector<vector<Pixel>>& image, double scaling_factor)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB values for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
            
            // get new color values by modifying original color values
            int new_red = 255 - (255 - red_value) * scaling_factor;
            int new_green = 255 - (255 - green_value) * scaling_factor;
            int new_blue = 255 - (255 - blue_value) * scaling_factor;
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;
        }
    }
    
    return new_image;
}


// darkens image by a scaling_factor
vector<vector<Pixel>> process_9(const vector<vector<Pixel>>& image, double scaling_factor)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB values for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
            
            // get new color values by modifying original color values
            int new_red = red_value * scaling_factor;
            int new_green = green_value * scaling_factor;
            int new_blue = blue_value * scaling_factor;
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;
        }
    }
    
    return new_image;
}


// converts image to only black, white, red, blue, and green
vector<vector<Pixel>> process_10(const vector<vector<Pixel>>& image)
{
    // get image height (num_rows) and width (num_columns)
    int num_rows = image.size();
    int num_columns = image[0].size();
    
    // create new image with same dimensions as input image
    vector<vector<Pixel>> new_image(num_rows, vector<Pixel>(num_columns));
    
    // iterate through each pixel of input image
    for (int row = 0; row < num_rows; ++row)
    {
        for (int col = 0; col < num_columns; ++col)
        {
            // get original RGB values for each pixel
            int red_value = image[row][col].red;
            int green_value = image[row][col].green;
            int blue_value = image[row][col].blue;
            
            // get max color value
            int max_color = max(max(red_value, green_value), blue_value);
            
            // get new color value based on sums of original color values and max_color
            int new_red = 0;
            int new_green = 0;
            int new_blue = 0;
            
            if (red_value + green_value + blue_value >= 550)
            {
                new_red = 255;
                new_green = 255;
                new_blue = 255;
            }
            else if (red_value + green_value + blue_value <= 150)
            {
                new_red = 0;
                new_green = 0;
                new_blue = 0;
            }
            else if (max_color == red_value)
            {
                new_red = 255;
                new_green = 0;
                new_blue = 0;
            }
            else if (max_color == green_value)
            {
                new_red = 0;
                new_green = 255;
                new_blue = 0;
            }
            else
            {
                new_red = 0;
                new_green = 0;
                new_blue = 255;
            }
            
            // map new color values to current pixel in new_image
            new_image[row][col].red = new_red;
            new_image[row][col].green = new_green;
            new_image[row][col].blue = new_blue;
        }
    }
    
    return new_image;
}


// ***** HELPER FUNCTIONS ***** //


// display image processing options
void display_menu(const string& filename)
{
    cout << endl;
    cout << "IMAGE PROCESSING MENU" << endl;
    cout << " 0) Change image (current: " << filename << ")" << endl;
    cout << " 1) Vignette" << endl;
    cout << " 2) Clarendon" << endl;
    cout << " 3) Grayscale" << endl;
    cout << " 4) Rotate 90 degrees" << endl;
    cout << " 5) Rotate multiple 90 degrees" << endl;
    cout << " 6) Enlarge" << endl;
    cout << " 7) High contrast" << endl;
    cout << " 8) Lighten" << endl;
    cout << " 9) Darken" << endl;
    cout << "10) Black, white, red, green, blue" << endl;
    cout << endl;
}


// get user menu selection
string get_menu_selection() 
{
    string menu_selection;
    cout << "Enter menu selection (Q to quit): ";
    cin >> menu_selection;
    return menu_selection;
}


// checks if input file exists - helper function for get_valid_file_name()
bool fileExists(const string& filename) 
{
    ifstream file(filename);
    return file.is_open();
}


// get name of existing input file
string get_valid_file_name()
{
    string file_name;  
    do {
        cout << "Enter input .bmp filename: ";
        cin >> file_name;
        if (!fileExists(file_name))
        {
            cout << "Error. File does not exist." << endl;
        }
    } while (!fileExists(file_name));
    
    return file_name;
}


// generate new image to output file
void generate_new_image(const vector<vector<Pixel>>& altered_image_vector, const string& process)
{
    cout << "Enter output .bmp filename: ";
    string output_file;
    cin >> output_file;
    if (write_image(output_file, altered_image_vector))
    {
        cout << "Successfully applied " << process << "!" << endl;
    }
    else
    {
        cout << "Failed process: " << process << endl;
    }      
}


// carry out process_1
void process_1_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Vignette selected" << endl;
    vector<vector<Pixel>> altered_image = process_1(image_vector);
    string process = "vignette";
    generate_new_image(altered_image, process);       
}


// carry out process_2
void process_2_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Clarendon selected" << endl;
    cout << "Enter scaling factor: ";
    double scaling_factor;
    cin >> scaling_factor;
    string process = "clarendon";
    vector<vector<Pixel>> altered_image = process_2(image_vector, scaling_factor);
    generate_new_image(altered_image, process);    
}


// carry out process_3
void process_3_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Grayscale selected" << endl;
    string process = "grayscale";
    vector<vector<Pixel>> altered_image = process_3(image_vector);
    generate_new_image(altered_image, process);    
}


// carry out process_4
void process_4_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Rotate 90 Degrees selected" << endl;
    vector<vector<Pixel>> altered_image = process_4(image_vector);
    string process = "rotate 90 degrees";
    generate_new_image(altered_image, process);   
}


// carry out process_5
void process_5_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Rotate Multiple 90 Degrees selected" << endl;
    cout << "Enter a positive integer: ";
    int rotation_num;
    cin >> rotation_num;
    vector<vector<Pixel>> altered_image = process_5(image_vector, rotation_num);
    string process = "rotate multiple 90 degrees";
    generate_new_image(altered_image, process);    
}


// carry out process_6
void process_6_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Enlarge selected" << endl;
    cout << "Enter X scaling factor (integer): ";
    int x_scale;
    cin >> x_scale;
    cout << "Enter Y scaling factor (integer): ";
    int y_scale;
    cin >> y_scale;
    vector<vector<Pixel>> altered_image = process_6(image_vector, x_scale, y_scale);
    string process = "enlarge";
    generate_new_image(altered_image, process);    
}


// carry out process_7
void process_7_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "High Contrast selected" << endl;
    vector<vector<Pixel>> altered_image = process_7(image_vector);
    string process = "high contrast";
    generate_new_image(altered_image, process);    
}


// carry out process_8
void process_8_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Lighten selected" << endl;
    cout << "Enter scaling factor: ";
    double scaling_factor;
    cin >> scaling_factor; 
    vector<vector<Pixel>> altered_image = process_8(image_vector, scaling_factor);
    string process = "lighten";
    generate_new_image(altered_image, process);
}


// carry out process_9
void process_9_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "Darken selected" << endl;
    cout << "Enter scaling factor: ";
    double scaling_factor;
    cin >> scaling_factor; 
    vector<vector<Pixel>> altered_image = process_9(image_vector, scaling_factor);
    string process = "darken";
    generate_new_image(altered_image, process);   
}


// carry out process_10
void process_10_helper(const vector<vector<Pixel>>& image_vector)
{
    cout << endl;
    cout << "BWRGB selected" << endl;
    vector<vector<Pixel>> altered_image = process_10(image_vector);
    string process = "BWRGB";
    generate_new_image(altered_image, process);  
}


// begin function main
int main()
{
    cout << endl;
    cout << "Image Processing Application" << endl;
    cout << endl;
    
    // prompt user for input file and check that file exists
    string file_name = get_valid_file_name();
        
    // read image file
    vector<vector<Pixel>> image_vector = read_image(file_name);
    
    // display menu and get user input
    display_menu(file_name);
    string menu_selection = get_menu_selection();
    
    // option selection
    while (menu_selection != "Q")
    {
        // change input file and get new image
        if (menu_selection == "0")
        {
            file_name = get_valid_file_name();
            image_vector = read_image(file_name);
        }
        // process 1
        else if (menu_selection == "1")
        {
            process_1_helper(image_vector);
        }
        // process 2
        else if (menu_selection == "2")
        {
            process_2_helper(image_vector);
        }
        // process 3
        else if (menu_selection == "3")
        {
            process_3_helper(image_vector);
        }
        // process 4
        else if (menu_selection == "4")
        {
            process_4_helper(image_vector);
        }
        // process 5
        else if (menu_selection == "5")
        {   
            process_5_helper(image_vector);
        }
        // process 6
        else if (menu_selection == "6")
        {
            process_6_helper(image_vector);
        }
        // process 7
        else if (menu_selection == "7")
        {
            process_7_helper(image_vector);
        }
        // process 8
        else if (menu_selection == "8")
        {
            process_8_helper(image_vector);
        }
        // process 9
        else if (menu_selection == "9")
        {
            process_9_helper(image_vector);
        }
        // process 10
        else if (menu_selection == "10")
        {
            process_10_helper(image_vector);
        }
        else
        {
            cout << endl;
            cout << "Invalid selection" << endl;
        }
         
        // display menu and get user input
        display_menu(file_name);
        menu_selection = get_menu_selection(); 
    }
    
    cout << endl;
    cout << "Goodbye." << endl;
    cout << endl;

    return 0;
}