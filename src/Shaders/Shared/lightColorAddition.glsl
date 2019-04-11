for(int i=0;i<dirLights;i++) {
	color += calcDirLight(dirLight[i], dir_shadows[i], fs_in.fragDirSpaces[i], diffuse.rgb, specular, viewDir);
}
for(int i=0;i<spotLights;i++) {
	color += calcSpotLight(spotLight[i], spot_shadows[i], fs_in.fragSpotSpaces[i], diffuse.rgb, specular, viewDir);
}
for(int i=0;i<pointLights;i++) {
	color += calcPointLight(pointLight[i], point_shadows[i], diffuse.rgb, specular, viewDir);
}