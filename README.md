# Pixel Sum algorithm

Test task: implementation of Integral image algorithm by class Pixel Sum 

## Essay

1. Typical buffers in our use cases are extremely sparse matrices, i.e., the vast majority of the
   pixels are zero. How would you exploit this to optimize the implementation?
   **A:** This is difficult without knowing the exact pattern how the algorithm can be adapted to use this images. There are several options:

   1. **Uniform image.** 

      ```
      `v, 0, 0, v, 0, 0, 0`
      `0, v, 0, 0, v, 0, 0`
      `0, 0, v, 0, 0, v, 0`
      `0, 0, 0, v, 0, 0, v`
      ```

      I see no obvious solutions how the algorithm can be optimized using this data

   2. **Clusters.** bounding box approach can be used, each cluster can be interpreted as a separate image, removing as much zeroes as possible out of the image. Calculation will be the sum of all pixels across all clusters intersected by the window of interest. It can reduce memory consumption, at a cost of value calculation speed

   3. **Single cluster, localized data.** Single bounding box can be used. Remove as much zeroes as possible to reduce image size.

2. What changes to the code and the API need to be made if buffer dimensions can be >= 4096
   x 4096? Also, what would happen if instead of 8 bits per pixel we have 16 bits per pixel?
   **A:** Since our field is 4096 * 4096 (2^12 * 2^12) max and each cell is one Byte long the max number that can be stored in integral image if all cells contains 0xFF is: 

   ```
   2^12*2^12 * (2^8 - 1) ~ 2^(12+12+8) ~ 2^32 = 0xFF000000
   ```

   Max value of the pixelSum is close to *uint32_t* numerical limit, so this type should satisfy our purpose, to create summed table area.
   What if size restrictions are not 4096 per edge but bigger, since max value will be close to *uint32_t* max, this type will not work, we can add 8 more rows or columns in this case, but adding more will require data structure change. Max value will exceed maximum *uint32_t* number. In this case we should use wider types like *int64_t/uint64_t*. The same situation if cell size of initial image is Word instead of Byte.

3. What would change if we needed to find the **maximum value** inside the search window (instead of the sum or average)? You don't need to code this, but it would be interesting to know how you would approach this problem in general, and what is your initial estimate of its O-complexity.
   **A:** Naive solution will be to scan all pixels within the window to find max value. Complexity is n*m where n and m are sizes of the search window. With one detail, the algorithm can be stopped if we found 0xFF value which is maximum possible value.
   Another possible solution can use existing algorithms. Search window can be split on quads, the quads with 0 or close to 0 average non zero can be skipped, remaining quads can be used as another search windows, split on quads, and so on. In case of sparse data it will work fine, In the worst case we should scan entire original search window, in this case algorithm will take more time than naive one.

4. How would multi-threading change your implementation? What kinds of performance benefits would you expect?
   **A:** Multithreading will work only at the moment of summed table creation. so we can speedup only data upload part. Meanwhile data is highly dependent on each other. **Line n** depends from value on the same column of **line n-1**. New line thread should be started at the moment when at least one value is calculated on previous line, synchronization will be another problem here, threads should follow one by one without races:

   ```
   ->
   n-1, ->
   n-2, n-2, ->
   ```

   I wouldn't consider multithreaded solution here unless it is absolutely necessary.

5. Outline shortly how the class would be implemented on a GPU (choose any relevant
   technology, e.g. CUDA, OpenCL). No need to code anything, just a brief overview is sufficient.
   **A:** I'm not familiar with both of proposed API's I'd explain changes in class, using Direct X Compute mechanisms:

   Data can be stored as a 2D UAV (Image) of UINT32 on top of DX GPU buffer. During initialization inside Compute shader the buffer will be filled with summed table data, another UAV will be filled with non-zero data. Support data like sizes will be stored as root signature constants (DX12). Data retrieving can be implemented inside Pixel or another Compute shader, depending on usage of the algorithm. 

   Copy Constructor and Assignment operator will be changes as well, simple buffer transfer cannot be done here. It should be copied with all support data using DX copy algorithms.
   The only complication here, if we are going to use the buffer or calculations results on CPU it should be transferred from GPU memory to CPU memory. 

## Task feedback

Some thoughts about task. I have set of small comments/concerns about the provided interface

1. Constructor uses signed type for sizes of the buffer, it might lead to unintentional errors and requires additional input validation on negative values. I'd consider to use size_t type for this purpose

   ```C++
   PixelSum(const unsigned char* buffer, size_t xWidth, size_t yHeight);
   ```

2. Coordinates input type. Signed type is used here, meanwhile task didn't mention any usage of negative values here. Is it really possible to provide negative coordinates to calculation functions?

   ```C++
   unsigned int getPixelSum(int x0, int y0, int x1, int y1) const;
   ```

   If the situation is not possible, I'd recommend to use unsigned type here to reduce number of input validations, unsigned int or uint32_t.

   ```c++
   uint32_t getPixelSum(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) const;
   ```

3. Type inconsistencies for the functions that provide the same type of data. getPixelSum and getNonZeroCount should provide the data of the same type, since they produce similar accumulated values. Otherwise we should expect from getNonZeroCount negative values and react appropriately. 

   ```C++
   unsigned int getNonZeroCount(int x0, int y0, int x1, int y1) const;
   ```

   
