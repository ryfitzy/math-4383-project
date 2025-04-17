import * as THREE from 'three';

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera( 75, window.innerWidth / window.innerHeight, 0.1, 1000 );

const renderer = new THREE.WebGLRenderer();
renderer.setSize( window.innerWidth, window.innerHeight );
renderer.setAnimationLoop( animate );
document.body.appendChild( renderer.domElement );

// Create a sphere with a reasonable number of segments
const radius = 2;
const segments = 64; // Number of longitude and latitude divisions
const geometry = new THREE.SphereGeometry(radius, segments, segments);
const material = new THREE.MeshLambertMaterial({ color: 0x00ff00 });
const sphere = new THREE.Mesh(geometry, material);
scene.add(sphere);


const light = new THREE.PointLight(0xffffff, 100, 100);
light.position.set(10, 10, 10); // Position the light source
scene.add(light);

camera.position.z = 10;



const patches = [];
const patchSize = Math.PI / 16; // Define the angular size of each patch (latitude and longitude divisions)

// Loop over latitude and longitude lines to create patches
for (let lat = -Math.PI / 2; lat < Math.PI / 2; lat += patchSize) {
    for (let lon = 0; lon < 2 * Math.PI; lon += patchSize) {
        const geometry = new THREE.PlaneGeometry(1, 1); // Placeholder for the patch geometry
        const patchMaterial = new THREE.MeshLambertMaterial({ color: 0x00ff00 });
        const patch = new THREE.Mesh(geometry, patchMaterial);

        // Convert spherical coordinates to Cartesian coordinates for positioning the patches
        const x = radius * Math.cos(lat) * Math.cos(lon);
        const y = radius * Math.cos(lat) * Math.sin(lon);
        const z = radius * Math.sin(lat);

        patch.position.set(x, y, z);

        // Add the patch to the scene and the patches array
        scene.add(patch);
        patches.push(patch);
    }
}

// Initialize radiosity for each patch (assuming emission is initially 0)
patches.forEach(patch => patch.radiosity = 0);

// Set emission values for light source patches
const lightPatches = patches.filter(patch => patch.material.color.equals(new THREE.Color(1, 1, 1))); // Example: identify light-emitting patches
lightPatches.forEach(patch => patch.radiosity = 1.0); // Set an initial emission for light sources

const maxIterations = 50;
for (let iteration = 0; iteration < maxIterations; iteration++) {
    const newRadiosities = [];
    
    for (let i = 0; i < patches.length; i++) {
        let incomingEnergy = 0;
        
        // Sum energy from all other patches
        for (let j = 0; j < patches.length; j++) {
            if (i !== j) {
                const F_ij = calculateFormFactor(patches[i], patches[j]);
                incomingEnergy += F_ij * patches[j].radiosity;
            }
        }
        
        // Update the radiosity of patch i
        const formFactorSum = patches.reduce((sum, patch, j) => sum + calculateFormFactor(patches[i], patch), 0);
        newRadiosities[i] = (patches[i].emission + incomingEnergy) / (1 + formFactorSum);
    }

    // Apply the new radiosities to the patches
    patches.forEach((patch, i) => patch.radiosity = newRadiosities[i]);
}

patches.forEach(patch => {
    // Scale the radiosity value to control the material color intensity
    const radiosityColor = new THREE.Color(1, 1, 1).multiplyScalar(patch.radiosity);
    patch.material.color.set(radiosityColor);
});

function animate() {

	renderer.render( scene, camera );

}