import { strToDataView } from "./util.js";

const items = new Map();

const socket = new WebSocket("ws://localhost:3000/ws");

socket.binaryType = "arraybuffer";

const itemInput = document.getElementById("itemInput");
const addItemButton = document.getElementById("addItem");
const errorItemExists = document.getElementById("errorExists");

itemInput.onkeydown = (event) => {
  if (event.key === "Enter") {
    addItemButton.click();
  }
}

addItemButton.onclick = () => {
  errorItemExists.style.visibility = "hidden";

  const name = itemInput.value;
  if (name === "") {
    return;
  }

  itemInput.value = "";

  if (items.has(name)) {
    errorItemExists.style.visibility = "visible";

    return;
  }

  addItem(name);
}

const addItem = (name, quantity = 1) => {
  const itemP = document.createElement("span");
  const quantityP = document.createElement("span");
  const valueInput = document.createElement("input");
  const valueButton = document.createElement("button");
  const deleteButton = document.createElement("button");

  itemP.classList.add("item");
  quantityP.classList.add("quantity");
  valueInput.classList.add("valueInput");
  valueButton.classList.add("valueButton");
  deleteButton.classList.add("deleteButton");

  itemP.innerText = name + ":";
  quantityP.innerText = quantity;

  valueInput.type = "number";
  valueInput.min = 1;

  valueButton.innerText = "Confirm quantity";
  deleteButton.innerText = "Delete item";

  const container = document.createElement("div");
  container.classList.add("container");
  container.appendChild(itemP);
  container.appendChild(quantityP);
  container.appendChild(valueInput);
  container.appendChild(valueButton);
  container.appendChild(deleteButton);

  valueInput.oninput = () => {
    valueInput.validity.valid || (valueInput.value = "");
  }

  valueInput.onkeydown = (event) => {
    if (event.key === "Enter") {
      valueButton.click();
    }
  }

  valueButton.onclick = () => {
    if (valueInput.value === "") {
      return;
    }

    const updatedQuantity = valueInput.value;

    quantityP.innerText = updatedQuantity;
    valueInput.value = "";

    updateItem(name, updatedQuantity);
  }

  deleteButton.onclick = () => {
    document.body.removeChild(container);
    updateItem(name, 0);
  }

  if (!updateItem(name, quantity)) {
    return;
  }

  document.body.insertBefore(container, document.getElementById("modContainer"));
}

const updateItem = (name, quantity) => {
  const array = new Uint8Array(4 + name.length + 4)
  const view = new DataView(array.buffer);

  view.setUint32(0, name.length, true);
  strToDataView(view, 4, name);
  view.setUint32(4 + name.length, quantity, true);

  if (socket.readyState === 1) {
    socket.send(view.buffer);
    quantity > 0 ? items.set(name, quantity) : items.delete(name);

    return true;
  }

  return false;
}

socket.onmessage = (message) => {
  const buffer = message.data;
  const view = new DataView(buffer);

  let offset = 0;

  const length = view.getUint32(offset, true);
  offset += 4;

  for (let i = 0; i < length; i++) {
    const nameLength = view.getUint32(offset, true);
    offset += 4;

    const item = new TextDecoder().decode(buffer.slice(offset, offset + nameLength));
    offset += nameLength;

    const quantity = view.getUint32(offset, true);
    offset += 4;

    addItem(item, quantity);
  }
}
