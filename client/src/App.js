import react, { useEffect, useState } from "react";
import mqtt from "mqtt";
import "./App.css";

import {
  Table,
  Modal,
  Form,
  InputGroup,
  FormControl,
  Button,
} from "react-bootstrap";
import "bootstrap/dist/css/bootstrap.min.css";

var client;

function App() {
  const [devices, setDevices] = useState([]);

  const [fullscreen, setFullscreen] = useState(true);
  const [show, setShow] = useState(false);

  useEffect(() => {
    client = mqtt.connect("mqtt://broker.hivemq.com:8000/mqtt");

    client.on("connect", function () {
      console.log("Conectei");
      client.subscribe("fse2021/170021751/#", function (err) {});
    });

    client.on("message", function (topic, message) {
      message = JSON.parse(message.toString());

      if (topic.includes("dispositivos")) {
        console.log(topic, message);
        setDevices([...devices, message.data]);

        handleShow("xl-down");
      }
    });
  }, []);

  function handleShow(breakpoint) {
    setFullscreen(breakpoint);
    setShow(true);
  }

  useEffect(() => {
    console.log(devices);
  }, [devices]);

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
                  Nome do Dispositivo
                </InputGroup.Text>
                <FormControl
                  aria-label="room_name"
                  aria-describedby="inputGroup-sizing-default"
                />
              </InputGroup>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Mac Address
                </InputGroup.Text>
                <FormControl
                  aria-label="room_name"
                  aria-describedby="inputGroup-sizing-default"
                />
              </InputGroup>
              <InputGroup className="mb-3">
                <InputGroup.Text id="inputGroup-sizing-default">
                  Comodo
                </InputGroup.Text>
                <FormControl
                  aria-label="room_name"
                  aria-describedby="inputGroup-sizing-default"
                />
              </InputGroup>
            </Form>
          </Modal.Body>
          <Modal.Footer>
            <Button varian="primary">Save</Button>
          </Modal.Footer>
        </Modal>

        <Table striped bordered hover>
          <tr>
            <th>Name</th>
            <th>Mac Address</th>
            <th>Room</th>
          </tr>
          {devices.map((device) => (
            <tr>
              <td>{device}</td>
              <td>Name</td>
            </tr>
          ))}
        </Table>
      </>
    </div>
  );
}

export default App;
