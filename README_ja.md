# filmQA_netOD_normalizer
netOD 等価の TIFF 画像を出力するソフトウェアです。

Net optical density (netOD) は下式であらわされます。
$$
\begin{align*}
OD &= log_{10}(\cfrac{2^{16}-1}{PV}) = log_{10}(65535/PV) \\

netOD &= OD_{exposure} - OD_{unexposure} \\
&= log_{10}(65535/PV_{exposure}) - log_{10}(65535/PV_{unexposure}) \\
&= log_{10}(\cfrac{65535/PV_{exposure}}{65535/PV_{unexposure}})  \\
&= log_{10}(\cfrac{PV_{unexposure}}{PV_{exposure}})
\end{align*}
$$
netOD は小数値ですが、一般的なフィルム解析ソフトウェアは画素値が整数値の画像を入力とするため、本ソフトウェアが出力するnetOD 等価画像は次式で表される画素値を持ちます。
$$
netOD_{equiv} = \cfrac{PV_{exposure}}{\overline{PV_{unexposure}}}\times65535
$$
必要であれば、Calibration Curve 取得時の既知線量で正規化することもできます。その場合、出力される netOD 等価画像の画素値は以下の式であらわされます。
$$
netOD_{equiv} = \cfrac{PV_{exposure}}{\overline{PV_{unexposure}}}\times65535\times k_{norm}\\ 
\text{where}\qquad
k_{norm} = \cfrac{\overline{PV_{calibration}}}{\overline{PV_{current}}}
$$


## 環境

- OS: Windows7, 11

- Qt5.15.2, Qt 6.6.0

- OpenCV 4.8.0

- LibTIFF 4.6.0



## 使用方法

1. 画像入力エリアに画像をドラッグ・アンド・ドロップまたはダブル・クリックして、平均画像を作成したいファイルを選択します。すると、入力画像の平均画像が表示されます。

2. netOD 画像を作成したい領域に ROI を設定します（赤い破線で表示）。

3. 未照射フィルムの均一な領域に ROI を設定します（青い点線で表示）。

4. (このステップは "Skip setting this ROI. "チェックボックスをチェックすることで省略できます)。

   必要であれば、Calibration Curve 取得時に既知の線量が照射されたフィルムに対して正規化を行うために、同じ線量が照射されたフィルムの均一な領域に ROI を設定します（緑色の一点短鎖線で表示）。次に、新しいウィンドウが表示されるので、Calibration Curve 取得時のフィルムに対して上記のステップ1～3を実行します。

5. "Write to: OUTPUT_PATH" ボタンをクリックして、netOD 画像を出力します。



## なぜ必要か？

生のピクセル値ではなく netOD を使用することで、スキャン間のばらつきによる線量の不確かさを低減することができます。



## 参考

- Niroomand-Rad, A., Chiu-Tsao, S.-T., Grams, M.P., Lewis, D.F., Soares, C.G., Van Battum, L.J., Das, I.J., Trichter, S., Kissick, M.W., Massillon-JL, G., Alvarez, P.E. and Chan, M.F. (2020), Report of AAPM Task Group 235 Radiochromic Film Dosimetry: An Update to TG-55. Med. Phys., 47: 5986-6025. https://doi.org/10.1002/mp.14497
