import { useState } from 'react';

const SERVICE_UUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const CHARACTERISTIC_UUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';

function App() {
  const [ecgValue, setEcgValue] = useState('Not connected');
  const [connected, setConnected] = useState(false);

  const connectToBLE = async () => {
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ name: 'Nano ESP2' }],
        optionalServices: [SERVICE_UUID],
      });

      const server = await device.gatt.connect();
      const service = await server.getPrimaryService(SERVICE_UUID);
      const characteristic = await service.getCharacteristic(CHARACTERISTIC_UUID);

      await characteristic.startNotifications();
      characteristic.addEventListener('characteristicvaluechanged', (event) => {
        const value = new TextDecoder().decode(event.target.value);
        setEcgValue(value);
      });

      setConnected(true);
    } catch (error) {
      console.error('Connection failed', error);
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
    </div>
  );
}

export default App;
