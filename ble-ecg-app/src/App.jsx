import { useState } from 'react';

const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const CHARACTERISTIC_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';
const WRITE_CHARACTERISTIC_UUID = '6e400003-b5a3-f393-e0a9-e50e24dcca9e';

function App() {
  const [ecgValue, setEcgValue] = useState('Not connected');
  const [connected, setConnected] = useState(false);
  const [errorMessage, setErrorMessage] = useState('');

  const connectToBLE = async () => {
    try {
      console.log('Requesting Bluetooth Device...');
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ name: 'Nano ESP32 ECG' }],
        optionalServices: [SERVICE_UUID],
      });
  
      console.log('Connecting to GATT Server...');
      const server = await device.gatt.connect();
      console.log('Connected to GATT Server.');
  
      console.log('Getting primary service...');
      const service = await server.getPrimaryService(SERVICE_UUID);
      console.log('Primary service obtained:', service);
  
      console.log('Getting ECG characteristic...');
      const characteristic = await service.getCharacteristic(CHARACTERISTIC_UUID);
      console.log('ECG characteristic found:', characteristic);
  
      await characteristic.startNotifications();
      characteristic.addEventListener('characteristicvaluechanged', (event) => {
        const value = new TextDecoder().decode(event.target.value);
        console.log('Received ECG Value:', value);
        setEcgValue(value);
      });
  
      setConnected(true);
      setErrorMessage('');
    } catch (error) {
      console.error('Connection failed:', error);
      setErrorMessage(error.message);
      setEcgValue('Error connecting');
    }
  };
  

  return (
    <div style={{ padding: '2rem', fontFamily: 'Arial' }}>
      <h1>Live ECG Monitor</h1>
      <button onClick={connectToBLE}>
        {connected ? 'Connected' : 'Connect to ESP32'}
      </button>
      <p style={{ marginTop: '1rem' }}>ECG Value: <strong>{ecgValue}</strong></p>
      {errorMessage && <p style={{ color: 'red' }}>Error: {errorMessage}</p>}
    </div>
  );
}

export default App;

