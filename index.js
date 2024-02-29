// import { initializeApp } from './node_modules/firebase/app';
// import { getFirestore, collection, getDocs, } from './node_modules/firebase/firestore/lite';
// import { getDatabase, ref, set } from "./node_modules/firebase/database";

// import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.1/firebase-app.js";
// import { getDatabase, ref, set, get, child } from "https://www.gstatic.com/firebasejs/10.8.1/firebase-database.js";

// // TODO: Replace the following with your app's Firebase project configuration
// // For Firebase JS SDK v7.20.0 and later, measurementId is optional
// const firebaseConfig = {
//   apiKey: "AIzaSyDkgzfbSwxVNdG7SjTEhHiE7bDNneHbyns",
//   authDomain: "instragramclone-d7409.firebaseapp.com",
//   projectId: "instragramclone-d7409",
//   storageBucket: "instragramclone-d7409.appspot.com",
//   messagingSenderId: "104506191011",
//   appId: "1:104506191011:web:fc08b4b8efa2b57e0da316",
//   measurementId: "G-9VMJB8RKF8",
//   databaseURL: "https://instragramclone-d7409-default-rtdb.firebaseio.com/"
// };

// const app = initializeApp(firebaseConfig);

// // const db = getFirestore(app);

// // Initialize Realtime Database and get a reference to the service
// const database = getDatabase(app);
// const heartBeat = document.getElementById("valueContainer").value;

// function writeUserData(heartBeat) {
//   set(ref(database, 'heartBeat/' + 'elderly'), {
//     heartBeat: heartBeat
//   });
//   alert('saved succefully');
// }

// writeUserData(heartBeat);

// // Get a list of cities from your database
// async function getCities(db) {
//   const citiesCol = collection(db, 'cities');
//   const citySnapshot = await getDocs(citiesCol);
//   const cityList = citySnapshot.docs.map(doc => doc.data());
//   return cityList;
// }


// Initialize and add the map
let map;

async function initMap() {
  // The location of Uluru
  const position = { lat: -25.344, lng: 131.031 };
  // Request needed libraries.
  //@ts-ignore
  const { Map } = await google.maps.importLibrary("maps");
  const { AdvancedMarkerElement } = await google.maps.importLibrary("marker");

  // The map, centered at Uluru
  map = new Map(document.getElementById("map"), {
    zoom: 4,
    center: position,
    mapId: "DEMO_MAP_ID",
  });

  // The marker, positioned at Uluru
  const marker = new AdvancedMarkerElement({
    map: map,
    position: position,
    title: "Uluru",
  });
}

initMap();

// to read data
// import { getDatabase, ref, onValue } from "firebase/database";

// const db = getDatabase();
// const starCountRef = ref(db, 'posts/' + postId + '/starCount');
// onValue(starCountRef, (snapshot) => {
//   const data = snapshot.val();
//   updateStarCount(postElement, data);
// });

