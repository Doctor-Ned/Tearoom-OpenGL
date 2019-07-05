for (int i = 0; i < dirLights; i++) {
	if (dirLight[i].enabled) {
		color += calcDirLight(dirLight[i], dir_shadows[i], fs_in.fragDirSpaces[i], albedoRGB, roughness, metallic, ao, N, V);
	}
}
for (int i = 0; i < spotLights; i++) {
	if (spotLight[i].enabled) {
		color += calcSpotLight(spotLight[i], spot_shadows[i], fs_in.fragSpotSpaces[i], albedoRGB, roughness, metallic, ao, N, V);
	}
}
for (int i = 0; i < pointLights; i++) {
	if (pointLight[i].enabled) {
		color += calcPointLight(pointLight[i], point_shadows[i], albedoRGB, roughness, metallic, ao, N, V);
	}
}