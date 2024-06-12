const LOG_ID = 0;
const LIGHT_ON = 1;
const TIMESTAMP = 2;
const METHOD = 3;

const logBox = document.querySelector(".log-box");

// use this link for local testing
// const link = "http://127.0.0.1:5000";

// use this link for production
const link = "http://13.52.247.21:5000";

async function turnOnLights() {
    const response = await fetch(`${link}/lights-on`);
    console.log(response.status);
    await logTurnOnLights();
    await displayLogs();
}

async function logTurnOnLights() {
    const response = await fetch(`${link}/create-log?light-on=1&method=button`, {
        method: 'POST'
    })
    console.log(response.status);
}

async function turnOffLights() {
    const response = await fetch(`${link}/lights-off`);
    console.log(response.status);
    await logTurnOffLights();
    await displayLogs();
}

async function logTurnOffLights() {
    const response = await fetch(`${link}/create-log?light-on=0&method=button`, {
        method: 'POST'
    })
    console.log(response.status);
}

async function displayLogs() {
    logBox.innerHTML = "";
    const logs = await getLogs();
    for (const log of logs) {
        const logDiv = createLog(log[LIGHT_ON], log[TIMESTAMP], log[METHOD]);
        logBox.insertBefore(logDiv, logBox.firstChild);
    }
}

async function getLogs() {
    const response = await fetch(`${link}/logs`);
    const data = await response.json();
    return data.result;
}

function createLog(light_on, timestamp, method) {
    const logDiv = document.createElement("div");
    logDiv.classList.add("log");

    const lightOnP = document.createElement("p");
    lightOnP.classList.add("light-on");
    lightOnP.textContent = light_on ? "light turned on" : "light turned off";

    const timestampP = document.createElement("p");
    timestampP.classList.add("timestamp");
    timestampP.textContent = timestamp;

    const methodP = document.createElement("p");
    methodP.classList.add("method")
    methodP.textContent = `method: ${method}`

    logDiv.appendChild(lightOnP);
    logDiv.appendChild(timestampP);
    logDiv.appendChild(methodP);

    return logDiv;
}

displayLogs()