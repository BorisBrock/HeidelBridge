/*
 * Modal
 *
 * Pico.css - https://picocss.com
 * Copyright 2019-2024 - Licensed under MIT
 */

// Config
const isOpenClass = "modal-is-open";
const openingClass = "modal-is-opening";
const closingClass = "modal-is-closing";
const scrollbarWidthCssVar = "--pico-scrollbar-width";
const animationDuration = 400; // ms
let visibleModal = null;

// Toggle modal
const toggleModal = (eventOrId) => {
    let modalId;
    if (typeof eventOrId === "string") {
        modalId = eventOrId;
    } else {
        eventOrId.preventDefault();
        modalId = eventOrId.currentTarget.dataset.target;
    }

    const modal = document.getElementById(modalId);
    if (!modal) return;
    modal.open ? closeModal(modal) : openModal(modal);
};

// Open modal with either an element or an ID string
const openModal = (modal) => {
    if (typeof modal === "string") {
        modal = document.getElementById(modal);
        if (!modal) {
            console.error(`Modal with ID '${modal}' not found.`);
            return;
        }
    }

    const { documentElement: html } = document;
    const scrollbarWidth = getScrollbarWidth();
    if (scrollbarWidth) {
        html.style.setProperty(scrollbarWidthCssVar, `${scrollbarWidth}px`);
    }
    html.classList.add(isOpenClass, openingClass);

    setTimeout(() => {
        visibleModal = modal;
        html.classList.remove(openingClass);
    }, animationDuration);

    modal.showModal();
};

// Close modal with either an element or an ID string
const closeModal = (modal) => {
    if (typeof modal === "string") {
        modal = document.getElementById(modal);
        if (!modal) {
            console.error(`Modal with ID '${modal}' not found.`);
            return;
        }
    }

    visibleModal = null;
    const { documentElement: html } = document;
    html.classList.add(closingClass);

    setTimeout(() => {
        html.classList.remove(closingClass, isOpenClass);
        html.style.removeProperty(scrollbarWidthCssVar);
        modal.close();
    }, animationDuration);
};


// Close with a click outside
/*document.addEventListener("click", (event) => {
    if (visibleModal === null) return;
    const modalContent = visibleModal.querySelector("article");
    const isClickInside = modalContent.contains(event.target);
    !isClickInside && closeModal(visibleModal);
});

// Close with Esc key
document.addEventListener("keydown", (event) => {
    if (event.key === "Escape" && visibleModal) {
        closeModal(visibleModal);
    }
});*/

// Get scrollbar width
const getScrollbarWidth = () => {
    const scrollbarWidth = window.innerWidth - document.documentElement.clientWidth;
    return scrollbarWidth;
};

// Is scrollbar visible
const isScrollbarVisible = () => {
    return document.body.scrollHeight > screen.height;
};