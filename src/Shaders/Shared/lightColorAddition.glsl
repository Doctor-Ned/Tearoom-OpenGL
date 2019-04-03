for(int i=0;i<dirLights;i++) {
	color += calcDirLight(dirLight[i], dir_shadows[i], fs_in.fragDirSpaces[i], diffuse, specular, viewDir);
}
for(int i=0;i<spotLights;i++) {
	color += calcSpotLight(spotLight[i], spot_shadows[i], fs_in.fragSpotSpaces[i], diffuse, specular, viewDir);
}
for(int i=0;i<pointLights;i++) {
	color += calcPointLight(pointLight[i], point_shadows[i], diffuse, specular, viewDir);
	//samplerCube tex = 0;
	//switch (i) {
	//	case 0:
	//		tex = point_shadow0;
	//	break;
	//	case 1:
	//		tex = point_shadow1;
	//		break;
	//	case 2:
	//		tex = point_shadow2;
	//		break;
	//	case 3:
	//		tex = point_shadow3;
	//		break;
	//}
	//color += calcPointLight(pointLight[i], tex, diffuse, specular, viewDir);
}