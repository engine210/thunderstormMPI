args = getArgument();
parts = split(args, ',');

open("/home/sc20/engine210/brc/data/layer_" + parts[0] + ".tif");
run("Run analysis", "filter=[Wavelet filter (B-Spline)] scale=2.0 order=3 detector=[Local maximum] connectivity=8-neighbourhood threshold=1.5*std(Wave.F1) estimator=[PSF: Integrated Gaussian] sigma=1.6 fitradius=4 method=[Maximum likelihood] full_image_fitting=false mfaenabled=false renderer=[Averaged shifted histograms] magnification=5.0 colorize=false threed=false shifts=2 repaint=50");
run("Export results", "floatprecision=1 filepath=" + "/home/sc20/engine210/brc/data/layer_" + parts[0] + ".csv" + " fileformat=[CSV (comma separated)] sigma=true intensity=true offset=true saveprotocol=false x=true y=true bkgstd=true id=false uncertainty_xy=true frame=true");
run("Quit");
