import { useEffect, useState } from "react";
import mqtt from "mqtt";

import {
  Table,
  Modal,
  Form,
  InputGroup,
  FormControl,
  Button,
  Accordion,
} from "react-bootstrap";
import "bootstrap/dist/css/bootstrap.min.css";

import alarmSound from "./assets/alarm.mp3";

var client;

function App() {
  const [devices, setDevices] = useState([]);

  const rooms = ["quarto", "sala", "cozinha", "banheiro", "varanda"];

  const [deviceMacAddress, setDeviceMacAddress] = useState("");
  const [deviceInput, setDeviceInput] = useState("");
  const [deviceOutput, setDeviceOutput] = useState("");
  const [deviceRoom, setDeviceRoom] = useState("");
  const [deviceAlarmed, setDeviceAlarmed] = useState(false);

  const [fullscreen, setFullscreen] = useState(true);
  const [show, setShow] = useState(false);

  const [deviceToActivate, setDeviceToActivate] = useState("");
  const [showActivationModal, setShowActivationModal] = useState(false);
  const [devicePotency, setDevicePotency] = useState(0);

  const studentId = "170021751";

  const playAlarm = (message, topic, device) => {
    if (topic !== "estado") return device;

    if (device.alarmed && message.data === "Activate") {
      const audio = new Audio(alarmSound);
      device.alarmRinging = true;
      audio.play();

      setTimeout(() => {
        audio.pause();
        device.alarmRinging = false;
        setDevices([...devices]);
      }, 2000);
    }
    return device;
  };

  const setDeviceData = (message, topic) => {
    const data = message.data;
    const macAddress = message.id;

    const _device = devices.filter((d) => d.macAddress === macAddress)[0];

    if (!_device) {
      return;
    }

    let device = devices.splice(devices.indexOf(_device), 1)[0];

    device[topic] = data;
    devices.push(device);

    device = playAlarm(message, topic, device);
    setDevices([...devices]);
  };

  useEffect(() => {
    client = mqtt.connect("mqtt://broker.hivemq.com:8000/mqtt");

    client.on("connect", function () {
      client.subscribe("fse2021/170021751/#", function (err) {});
    });

    client.on("message", function (topic, message) {
      message = JSON.parse(message.toString());

      if (topic.includes("dispositivos")) {
        if (message.command === "register") {
          setDeviceMacAddress(message.data);

          handleShow("xl-down");
        }
      } else {
        if (topic.includes("temperatura")) {
          setDeviceData(message, "temperature");
        } else if (topic.includes("umidade")) {
          setDeviceData(message, "humidity");
        } else if (topic.includes("estado")) {
          setDeviceData(message, "estado");
        }
      }
    });
  }, [devices]);

  function handleShow(breakpoint) {
    setFullscreen(breakpoint);
    setShow(true);
  }

  useEffect(() => {}, [devices]);

  const publishMessage = (command, data, _deviceMacAddress) => {
    const topic = `fse2021/${studentId}/dispositivos/${_deviceMacAddress}`;

    const message = {
      command: command,
      data: data,
    };

    client.publish(topic, JSON.stringify(message));
  };

  const cleanDeviceState = () => {
    setDeviceMacAddress("");
    setDeviceRoom("");
    setDeviceInput("");
    setDeviceOutput("");
    setDeviceAlarmed(false);
  };

  const registerDevice = () => {
    const device = {
      macAddress: deviceMacAddress,
      input: deviceInput,
      output: deviceOutput,
      room: deviceRoom || rooms[0],
      alarmed: deviceAlarmed,
    };

    setDevices([...devices, device]);
    publishMessage("allocated_room", device.room, device.macAddress);
    setShow(false);
    cleanDeviceState();
  };

  const openActivationModal = (deviceInput, _deviceMacAddress) => {
    setDeviceToActivate(deviceInput);
    setDeviceMacAddress(_deviceMacAddress);
    setShowActivationModal(true);
  };

  const activateDevice = () => {
    const potency = parseInt(devicePotency) / 100;
    publishMessage("led", potency.toString(), deviceMacAddress);
    setShowActivationModal(false);
  };

  const removeDevice = (macAddress) => {
    const answer = window.confirm("Tem certeza?");

    if (!answer) return;

    const _device = devices.filter((d) => d.macAddress === macAddress)[0];
    devices.splice(devices.indexOf(_device), 1);
    setDevices([...devices]);

    publishMessage("reset", "0", macAddress);
  };

  return (
    <div className="App">
      <>
        <Modal
          show={show}
          fullscreen={fullscreen}
          onHide={() => setShow(false)}
        >
          <Modal.Header closeButton>
            <Modal.Title>Cadastrar Novo Dispositivo</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <Form>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Dispositivo de Entrada
                </InputGroup.Text>
                <FormControl
                  aria-describedby="inputGroup-sizing-default"
                  value={deviceInput}
                  onChange={(event) => setDeviceInput(event.target.value)}
                />
              </InputGroup>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Dispositivo de Saída
                </InputGroup.Text>
                <FormControl
                  aria-describedby="inputGroup-sizing-default"
                  value={deviceOutput}
                  onChange={(event) => setDeviceOutput(event.target.value)}
                />
              </InputGroup>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Mac Address
                </InputGroup.Text>
                <FormControl
                  aria-describedby="inputGroup-sizing-default"
                  value={deviceMacAddress}
                  readOnly
                />
              </InputGroup>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Cômodo
                </InputGroup.Text>
                <Form.Select
                  aria-label="Cômodo"
                  value={deviceRoom}
                  onChange={(event) => setDeviceRoom(event.target.value)}
                >
                  {rooms.map((room, index) => {
                    return (
                      <option key={index} value={room}>
                        {room}
                      </option>
                    );
                  })}
                </Form.Select>
              </InputGroup>
              <Form.Check
                type="switch"
                label="Alarme"
                onChange={(event) => setDeviceAlarmed(event.target.checked)}
                value={deviceAlarmed}
              />
            </Form>
          </Modal.Body>
          <Modal.Footer>
            <Button varian="primary" onClick={() => registerDevice()}>
              Save
            </Button>
          </Modal.Footer>
        </Modal>

        <Modal
          show={showActivationModal}
          fullscreen={false}
          onHide={() => setShowActivationModal(false)}
        >
          <Modal.Header closeButton>
            <Modal.Title>{deviceToActivate}</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <Form.Label>Potência do dispositivo: {devicePotency}</Form.Label>
            <Form.Range
              min="0"
              max="100"
              onChange={(event) => setDevicePotency(event.target.value)}
              value={devicePotency}
            />
          </Modal.Body>
          <Modal.Footer>
            <Button varian="primary" onClick={() => activateDevice()}>
              Acionar
            </Button>
          </Modal.Footer>
        </Modal>

        <Accordion>
          {rooms.map((room, index) => {
            return (
              <Accordion.Item key={index} eventKey={index.toString()}>
                <Accordion.Header>{room}</Accordion.Header>
                <Accordion.Body>
                  <Table striped bordered hover>
                    <thead>
                      <tr>
                        <th>Dispositivo de Entrada</th>
                        <th>Dispositivo de Saída</th>
                        <th>Mac Address</th>
                        <th>Cômodo</th>
                        <th>Alarme</th>
                        <th>Temperatura</th>
                        <th>Umidade</th>
                        <th />
                      </tr>
                    </thead>
                    <tbody>
                      {devices
                        .filter((device) => device.room === room)
                        .map((device, index) => (
                          <tr key={index}>
                            <td>{device.input}</td>
                            <td>
                              {device.output}
                              {device.alarmRinging && (
                                <Button
                                  disabled
                                  className="btn mx-5 btn-danger"
                                />
                              )}
                              {!device.alarmRinging && (
                                <Button
                                  disabled
                                  className="btn mx-5 btn-success"
                                />
                              )}
                            </td>
                            <td>{device.macAddress}</td>
                            <td>{device.room}</td>
                            <td>{device.alarmed ? "Ativado" : "Desativado"}</td>
                            <td>
                              {device.temperature ? device.temperature : "--"}
                            </td>
                            <td>{device.humidity ? device.humidity : "--"}</td>
                            <td>
                              <Button
                                className="btn btn-sm"
                                onClick={() =>
                                  openActivationModal(
                                    device.input,
                                    device.macAddress
                                  )
                                }
                              >
                                Acionar Dispositivo
                              </Button>
                            </td>
                            <td>
                              <Button
                                onClick={() => removeDevice(device.macAddress)}
                                className="btn btn-sm btn-danger"
                              >
                                Remover
                              </Button>
                            </td>
                          </tr>
                        ))}
                    </tbody>
                  </Table>
                </Accordion.Body>
              </Accordion.Item>
            );
          })}
        </Accordion>
      </>
    </div>
  );
}

export default App;
