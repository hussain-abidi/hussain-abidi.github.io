const strToU8Array = (str) => {
  return new TextEncoder().encode(str);
}

const strToDataView = (view, offset, str) => {
  const byteArray = strToU8Array(str);
  for (let i = 0; i < byteArray.length; ++i) {
    view.setUint8(offset + i, byteArray[i]);
  }
}

export { strToDataView };
