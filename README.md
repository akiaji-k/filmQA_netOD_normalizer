# filmQA_netOD_normalizer
A GUI software to create netOD-equivalent TIFF image.

Net optical density (netOD) is represented by the following equation.


$$
\begin{align*}
OD &= log_{10}(\cfrac{2^{16}-1}{PV}) = log_{10}(65535/PV) \\

netOD &= OD_{exposure} - OD_{unexposure} \\
&= log_{10}(65535/PV_{exposure}) - log_{10}(65535/PV_{unexposure}) \\
&= log_{10}(\cfrac{65535/PV_{exposure}}{65535/PV_{unexposure}})  \\
&= log_{10}(\cfrac{PV_{unexposure}}{PV_{exposure}})
\end{align*}
$$
Although netOD is a fractional value, typical film analysis software takes an image with integer pixel values as input, so the netOD-equivalent image output by the software has pixel values represented by the following equation.
$$
netOD_{equiv} = \cfrac{PV_{exposure}}{\overline{PV_{unexposure}}}\times65535
$$
If necessary, normalization with the known dose at the time of Calibration Curve acquisition can be applied. In that case, the pixel values of the output netOD-equivalent image will be the following equation.
$$
netOD_{equiv} = \cfrac{PV_{exposure}}{\overline{PV_{unexposure}}}\times65535\times k_{norm}\\ 
\text{where}\qquad
k_{norm} = \cfrac{\overline{PV_{calibration}}}{\overline{PV_{current}}}
$$


## Environment

- OS: Windows7, 11

- Qt5.15.2, Qt 6.6.0

- OpenCV 4.8.0

- LibTIFF 4.6.0



## Usage

1. Drag and drop or double-click on the image input area to select a file(s) from which you want to create a mean image. Then the mean image of the input images will be displayed. 

2. Set an ROI to an area where you want to create a netOD image (indicated by the red dash line).

3. Set an ROI to a uniform area of unirradiated film (indicated by a blue dotted line). 

4. (This step can be skipped by checking the "Skip setting this ROI." checkbox.) 

  If necessary, to normalize to the film that was exposed to a known dose when the Calibration Curve was acquired, set an ROI to a uniform area of the film with a known dose (indicated by the green dash-dotted line). Then, in a new window that appears, perform steps 1-3 above for the film at the time of Calibration Curve acquisition.

5. Click on the button labeled "Write to: OUTPUT_PATH" to output the netOD images.



## Why is it needed?

Because the use of netOD instead of raw pixel values reduces dose uncertainty due to scan-to-scan variation.



## References

- Niroomand-Rad, A., Chiu-Tsao, S.-T., Grams, M.P., Lewis, D.F., Soares, C.G., Van Battum, L.J., Das, I.J., Trichter, S., Kissick, M.W., Massillon-JL, G., Alvarez, P.E. and Chan, M.F. (2020), Report of AAPM Task Group 235 Radiochromic Film Dosimetry: An Update to TG-55. Med. Phys., 47: 5986-6025. https://doi.org/10.1002/mp.14497
